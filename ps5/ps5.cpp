#include "ps5.hpp"
#include <cstdio>

namespace pad {
  namespace ps5 {
    namespace {
      // 内部でのみ使用するAxisID
      constexpr uint8_t internal_axis_crossX = 6;
      constexpr uint8_t internal_axis_crossY = 7;

      constexpr uint32_t STICK_MAX = 255;
      constexpr float RESOLUTION_ACC_PER_G = 8192.0f;
      constexpr float RESOLUTION_DEG_PER_S = 1024.0f; 

      auto is_gyro_id = [](int id) {
        return (id >= MotionAxisID::GyroX);
      };
    }

    PS5Handler::PS5Handler() {
      id_map_ = code_id_map {
        // for Button
        {BTN_SOUTH,  ButtonID::cross},
        {BTN_EAST,   ButtonID::circle},
        {BTN_NORTH,  ButtonID::triangle},
        {BTN_WEST,   ButtonID::square},
        {BTN_TL,     ButtonID::L1},
        {BTN_TR,     ButtonID::R1},
        {BTN_TL2,    ButtonID::L2},
        {BTN_TR2,    ButtonID::R2},
        {BTN_SELECT, ButtonID::create},
        {BTN_START,  ButtonID::option},
        {BTN_MODE,   ButtonID::ps},
        {BTN_THUMBL, ButtonID::L3},
        {BTN_THUMBR, ButtonID::R3},
        // for Axis 
        {ABS_X,  AxisID::leftX},
        {ABS_Y,  AxisID::leftY},
        {ABS_RX, AxisID::rightX},
        {ABS_RY, AxisID::rightY},
        {ABS_Z,  AxisID::L2depth},
        {ABS_RZ, AxisID::R2depth},
        {ABS_HAT0X,  internal_axis_crossX},
        {ABS_HAT0Y,  internal_axis_crossY},
      };

      this->deadzone_ = default_deadzone;
    }

    void PS5Handler::handleCrossXData(int32_t val) {
      if (val > 0) {
        this->pre_crossXid_ = ButtonID::right;
        button_event_ = { .id = ButtonID::right, .state = true};
      }
      else if (val < 0)  {
        this->pre_crossXid_ = ButtonID::left;
        button_event_ = { .id = ButtonID::left, .state = true};
      }
      else {
        switch (this->pre_crossXid_) {
          case (ButtonID::right): {
            button_event_.id = ButtonID::right; 
            break;
          }
          case (ButtonID::left): {
            button_event_.id = ButtonID::left;  
            break;
          }
        }
        button_event_.state = false;
      }
    }

    void PS5Handler::handleCrossYData(int32_t val) {
      if (val > 0) {
        this->pre_crossXid_ = ButtonID::down;
        button_event_ = { .id = ButtonID::down, .state = true};
      }
      else if (val < 0)  {
        this->pre_crossXid_ = ButtonID::up;
        button_event_ = { .id = ButtonID::up, .state = true};
      }
      else {
        switch (this->pre_crossXid_) {
          case (ButtonID::down): {
            button_event_.id = ButtonID::down; 
            break;
          }
          case (ButtonID::up): {  
            button_event_.id = ButtonID::up;  
            break;
          }
        }
        button_event_.state = false;
      }
    }

    void PS5Handler::handleAxisEvent() {
      int32_t val = event_.value;
      uint8_t id  = id_map_[event_.code]; 

      switch (id) {
        case (internal_axis_crossX): {
          event_.type = EventType::Button;
          handleCrossXData(val);
          return;
        }
        case (internal_axis_crossY): {
          event_.type = EventType::Button;
          handleCrossYData(val);
          return;
        }
      }

      // Stick 値の範囲を-STICK_MAX <--> STICK_MAX に拡張
      // origin: 0 ~ 255 -> -255 ~ 255
      if (id != AxisID::L2depth && id != AxisID::R2depth) {
        val = (val << 1) - STICK_MAX;
      }

      switch (id) {
        // Y軸の上側が+になるよう反転
        case (AxisID::leftY):
        case (AxisID::rightY): {
          val *= -1;
        }
        default: {
          // axis値を-1.0 <--> 1.0 に
          axis_event_.id = id;
          float fval = static_cast<float>(val) / STICK_MAX;
          if (fabs(fval) < deadzone_) 
            fval = 0.0;
          
          axis_event_.value = fval;
        }
      }
    }

    void PS5Handler::handleButtonEvent() {
      button_event_.id = id_map_[event_.code];
      button_event_.state = (event_.value == 1) ? true : false;
    }

    PS5MotionHandler::PS5MotionHandler() {
      id_map_ = code_id_map {
        {ABS_X,  MotionAxisID::AccelX},
        {ABS_Y,  MotionAxisID::AccelY},
        {ABS_Z,  MotionAxisID::AccelZ},
        {ABS_RX, MotionAxisID::GyroX},
        {ABS_RY, MotionAxisID::GyroY},
        {ABS_RZ, MotionAxisID::GyroZ},
      };

      std::printf("[PS5MotionHandler] please run with 'sudo' to read event.\n");
    }

    void PS5MotionHandler::handleEvent(EventReader& reader) {
      this->event_ = reader.getEvent();

      if (event_.type != EventType::Axis)
        return;

      this->axis_event_.id = id_map_[event_.code];

      int32_t val = event_.value;
      float fval;
      if (is_gyro_id(axis_event_.id)) {
        fval = static_cast<float>(val) / RESOLUTION_DEG_PER_S; 
      }
      else {
        fval = static_cast<float>(val) / RESOLUTION_ACC_PER_G;
      }

      this->axis_event_.value = fval;
    }
  }
}