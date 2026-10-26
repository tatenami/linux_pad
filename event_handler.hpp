#ifndef EVNET_HANDLER_H
#define EVNET_HANDLER_H

#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/input.h>
#include <sys/ioctl.h>

#include <unordered_map>

#include "event_types.hpp"
#include "event_reader.hpp"

namespace pad {

  constexpr float DEFAULT_DEADZONE = 0.05;
  constexpr int DEFAULT_BUTTON_NUM = 20;
  constexpr int DEFALUT_AXIS_NUM = 8;
  constexpr int DEFAUTL_MOTION_AXIS_NUM = 6;
  constexpr int MAX_EVENTS = 32;

  // デバイスファイルのCODE->インターフェース用ID の変換テーブルmap
  using code_id_map = std::unordered_map<uint32_t, uint8_t>;

  class PadEventHandler {
   protected:
    Event event_;
    code_id_map id_map_;
    ButtonEvent button_event_ = { .id = 0, .state = false};
    AxisEvent axis_event_ = { .id = 0, .value = 0.0f};
    float deadzone_{DEFAULT_DEADZONE};

    virtual void handleButtonEvent() = 0;
    virtual void handleAxisEvent() = 0;

   public:
    void handleEvent(EventReader& reader);
    void setDeadZone(float deadzone);

    EventType getEventType() { 
      return this->event_.type; 
    }

    ButtonEvent& getButtonEvent() {
      return this->button_event_;
    }

    AxisEvent& getAxisEvent() {
      return this->axis_event_;
    }    
  };

  class MotionEventHandler {
   protected:
    Event event_; 
    code_id_map id_map_;
    AxisEvent axis_event_;
    float deadzone_{DEFAULT_DEADZONE};

   public:
    virtual void handleEvent(EventReader& reader) = 0;

    AxisEvent& getAxisEvent() {
      return this->axis_event_;
    }
  };
}
#endif // EVNET_HANDLER_H