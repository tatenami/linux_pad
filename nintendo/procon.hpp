#ifndef ProController_H
#define ProController_H

#include "gamepad.hpp"
#include <cmath>
#include <cstdint>
#include <limits>

namespace pad {

  namespace procon {
    
    namespace Device {
      const std::string PAD_USB = "/dev/evdev_procon_usb";
      const std::string PAD_BT  = "/dev/evdev_procon_bt";    
    }

    const float default_deadzone = DEFAULT_DEADZONE;

    namespace ButtonID {
      const uint8_t B = 0;
      const uint8_t A = 1;
      const uint8_t X = 2;
      const uint8_t Y = 3;
      const uint8_t L = 4;
      const uint8_t R = 5;
      const uint8_t ZL = 6;
      const uint8_t ZR = 7;
      const uint8_t minus = 8;
      const uint8_t plus = 9;
      const uint8_t home = 10;
      const uint8_t cpature = 11;
      const uint8_t Ls = 12;
      const uint8_t Rs = 13;
      const uint8_t left = 14;
      const uint8_t right = 15;
      const uint8_t up = 16;
      const uint8_t down = 17;
    }

    namespace AxisID {
      const uint8_t leftX = 0;
      const uint8_t leftY = 1;
      const uint8_t rightX = 2;
      const uint8_t rightY = 3;
    }

    class ProControllerHandler: public PadEventHandler {
     private:
      uint8_t pre_crossXid_;
      uint8_t pre_crossYid_;
      uint32_t axis_max_;

      void handleCrossXData(int32_t val);
      void handleCrossYData(int32_t val);
      void handleButtonEvent() override;
      void handleAxisEvent() override;

     public:
      // GamePad にアタッチするハンドラの型として
      //  - static constexpr size_t button_count
      //  - static constexpr size_t axis_count 
      // が定義されている必要がある
      static constexpr size_t button_count = 18;
      static constexpr size_t axis_count = 6;

      ProControllerHandler();
    };
  }
}

#include "impl/procon_impl.ipp"

#endif // ProController_H