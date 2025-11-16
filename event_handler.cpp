#include "event_handler.hpp"

namespace pad {
  void PadEventHandler::setDeadZone(float deadzone) {
    this->deadzone_ = deadzone;
  }

  void PadEventHandler::handleEvent(EventReader& reader) {
    this->event_ = reader.getEvent();

    switch (event_.type) {
      case (EventType::Button): {
        handleButtonEvent();
        break;
      }
      case (EventType::Axis): {
        handleAxisEvent();
        break;
      }
      default:
        break;
    }
  }
  
}
