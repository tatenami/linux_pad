#ifndef PAD_H
#define PAD_H

#include "padbase.hpp"

namespace pad {

  template <typename Handler,
    typename = std::enable_if_t<std::is_base_of<PadEventHandler, Handler>::value>>
  class GamePad: public BasePad<Handler> {
   public:
    GamePad(std::string devfile_path,
            int button_num = default_button_num, 
            int axis_num = default_axis_num):
      BasePad<Handler>(devfile_path, button_num, axis_num)
    {
      // this->handler_ = std::make_unique<Handler>();
    }

    void update() {
      this->buttons_.resetUpdateFlag();
      BasePad<Handler>::update();
    }

    inline bool press(uint8_t id) {
      return this->buttons_.getState(id);
    }

    inline bool pushed(uint8_t id) {
      if (!this->buttons_.IDUpdated(id)) {
        return false;
      }

      if (this->buttons_.getState(id) == true) {
        return true;
      }
      else {
        return false;
      }
    }

    inline bool released(uint8_t id) {
      if (!this->buttons_.IDUpdated(id)) {
        return false;
      }

      if (this->buttons_.getState(id) == false) {
        return true;
      }
      else {
        return false;
      }
    }

    inline float axisValue(uint8_t id) {
      return this->axes_.getValue(id);
    }
  };
}

#endif // PAD_H