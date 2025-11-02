#ifndef PS5_BASE_H
#define PS5_BASE_H

#include "gamepad.hpp"

namespace pad {

  namespace ps5 {
    const std::string evdev_symlink_usb = "/dev/evdev_dualsense_usb";
    const std::string evdev_symlink_bt  = "/dev/evdev_dualsense_bt";

    namespace dev {
      constexpr uint8_t num_buttons = 17;
      constexpr uint8_t num_axes = 6;
    }

    const float default_deadzone = 0.05; 

    namespace ButtonID {
      constexpr uint8_t cross    = 0;
      constexpr uint8_t circle   = 1;
      constexpr uint8_t triangle = 2;
      constexpr uint8_t square   = 3;
      constexpr uint8_t L1       = 4;
      constexpr uint8_t R1       = 5;
      constexpr uint8_t L2       = 6;
      constexpr uint8_t R2       = 7;
      constexpr uint8_t create   = 8;
      constexpr uint8_t option   = 9;
      constexpr uint8_t ps       = 10;
      constexpr uint8_t L3       = 11;
      constexpr uint8_t R3       = 12;
      constexpr uint8_t left     = 13;
      constexpr uint8_t right    = 14;
      constexpr uint8_t up       = 15;
      constexpr uint8_t down     = 16;
    }

    namespace AxisID {
      constexpr uint8_t leftX   = 0;
      constexpr uint8_t leftY   = 1;
      constexpr uint8_t L2depth = 2;
      constexpr uint8_t rightX  = 3;
      constexpr uint8_t rightY  = 4;
      constexpr uint8_t R2depth = 5;
    }

    class PS5Handler: public PadEventHandler {
     private:
      uint8_t pre_crossXid_;
      uint8_t pre_crossYid_;
      int32_t axis_max_;

      void handleCrossXData(int32_t val);
      void handleCrossYData(int32_t val);
      void handleButtonEvent() override;
      void handleAxisEvent() override;

     public:
      PS5Handler();
    };
  }
}

#endif // XXX_H