#ifndef PS5_BASE_H
#define PS5_BASE_H

#include "gamepad.hpp"
#include <cmath>

namespace pad {

  namespace ps5 {

    namespace Device {
      const std::string PAD_USB = "/dev/evdev_ds_usb";
      const std::string PAD_BT  = "/dev/evdev_ds_bt";
      const std::string MOTION_USB = "/dev/evdev_ds_motion_usb";
      const std::string MOTION_BT  = "/dev/evdev_ds_motion_bt";
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

    namespace MotionAxisID {
      constexpr uint8_t AccelX = 0;
      constexpr uint8_t AccelY = 1;
      constexpr uint8_t AccelZ = 2;
      constexpr uint8_t GyroX = 3;
      constexpr uint8_t GyroY = 4;
      constexpr uint8_t GyroZ = 5;
    }

    class PS5Handler: public PadEventHandler {
     private:
      uint8_t pre_crossXid_;
      uint8_t pre_crossYid_;

      void handleCrossXData(int32_t val);
      void handleCrossYData(int32_t val);
      void handleButtonEvent() override;
      void handleAxisEvent() override;

     public:
      static constexpr size_t button_count = 17;
      static constexpr size_t axis_count = 6;

      PS5Handler();
    };

    class PS5MotionHandler: public MotionEventHandler {
     private:
      void handleEvent(EventReader& reader) override;

     public:
      PS5MotionHandler();
    };
  }
}

#include "impl/ps5_impl.ipp"

#endif // XXX_H