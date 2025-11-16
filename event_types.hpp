#ifndef EVENT_DEFS_H
#define EVENT_DEFS_H

#include <stdint.h>

namespace pad {
  enum class EventType {
    None, 
    Button, 
    Axis
  };

  struct Event {
    EventType type;
    uint16_t  code;
    int32_t   value;
  };

  struct ButtonEvent {
    uint8_t id;
    bool    state;
  };

  struct AxisEvent {
    uint8_t id;
    float   value;
  };
}
#endif // EVENT_DEFS_H