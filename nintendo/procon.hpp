#ifndef ProController_H
#define ProController_H

#include "pad/pad.hpp"

namespace pad {

  namespace procon {
    
    const std::string usb_name = "\"Nintendo Co., Ltd. Pro Controller\"";
    
    namespace dev {
      const int button_num = 18;
      const int axis_num = 4;
    }

    const float default_deadzone = 0.05;

    namespace ButtonID {
      const int B = 0;
      const int A = 1;
      const int X = 2;
      const int Y = 3;
      const int L = 4;
      const int R = 5;
      const int ZL = 6;
      const int ZR = 7;
      const int minus = 8;
      const int plus = 9;
      const int home = 10;
      const int cpature = 11;
      const int Ls = 12;
      const int Rs = 13;
      const int left = 14;
      const int right = 15;
      const int up = 16;
      const int down = 17;
    }

    namespace AxisID {
      const int leftX = 0;
      const int leftY = 1;
      const int rightX = 2;
      const int rightY = 3;
    }

    class ProControllerHandler: public PadEventHandler {
      private:
      int pre_crossXid_;
      int pre_crossYid_;
      uint32_t axis_max_;

      void handleCrossXData(int32_t val);
      void handleCrossYData(int32_t val);
      void handleButtonEvent() override;
      void handleAxisEvent() override;

      public:
      ProControllerHandler();
    };
  }
}

#endif // ProController_H