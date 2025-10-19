#include "procon.hpp"

namespace pad {
  
  namespace procon {

    namespace {
      // 内部でのみ使用するAxisID
      constexpr uint8_t internal_axis_crossX = 4;
      constexpr uint8_t internal_axis_crossY = 5;
    }

    ProControllerHandler::ProControllerHandler() {
      id_map_ = code_id_map {
        // for Button
        {BTN_SOUTH,  ButtonID::B},
        {BTN_EAST,   ButtonID::A},
        {BTN_NORTH,  ButtonID::X},
        {BTN_WEST,   ButtonID::Y},
        {BTN_TL,     ButtonID::L},
        {BTN_TR,     ButtonID::R},
        {BTN_TL2,    ButtonID::ZL},
        {BTN_TR2,    ButtonID::ZR},
        {BTN_SELECT, ButtonID::minus},
        {BTN_START,  ButtonID::plus},
        {BTN_MODE,   ButtonID::home},
        {BTN_Z,      ButtonID::cpature},
        {BTN_THUMBL, ButtonID::Ls},
        {BTN_THUMBR, ButtonID::Rs},
        // for Axis
        {ABS_X,   AxisID::leftX},
        {ABS_Y,   AxisID::leftY},
        {ABS_RX,  AxisID::rightX},
        {ABS_RY,  AxisID::rightY},
        {ABS_HAT0X,  internal_axis_crossX},
        {ABS_HAT0Y,  internal_axis_crossY},
      };

      this->axis_max_ = std::numeric_limits<int16_t>::max();
      this->deadzone_ = default_deadzone;
    }

    void ProControllerHandler::handleCrossXData(int32_t val) {
      if (val > 0) {
        this->pre_crossXid_ = ButtonID::right;
        button_event_ = {ButtonID::right, true};
      }
      else if (val < 0)  {
        this->pre_crossXid_ = ButtonID::left;
        button_event_ = {ButtonID::left, true};
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

    void ProControllerHandler::handleCrossYData(int32_t val) {
      if (val > 0) {
        this->pre_crossXid_ = ButtonID::down;
        button_event_ = {ButtonID::down, true};
      }
      else if (val < 0)  {
        this->pre_crossXid_ = ButtonID::up;
        button_event_ = {ButtonID::up, true};
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

    void ProControllerHandler::handleAxisEvent() {
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

      switch (id) {
        case (AxisID::leftY):
        case (AxisID::rightY): {
          val *= -1;
        }
        default: {
          axis_event_.id = id;
          float fval = static_cast<float>(val) / axis_max_;
          if (fabs(fval) < deadzone_) fval = 0.0;
          axis_event_.value = fval;
        }
      }
    }

    void ProControllerHandler::handleButtonEvent() {
      button_event_.id = id_map_[event_.code];

      switch (event_.value) {
        case 0: {
          button_event_.state = false;
          break;
        }
        case 1: {
          button_event_.state = true;
          break;
        }
      }
    }

  }

}