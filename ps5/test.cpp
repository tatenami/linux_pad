#include "ps5pad.hpp"
#include <cstdio>

using namespace pad;
using namespace std;

int main() {
  GamePad<ps5::PS5Handler> ps5(ps5::evdev_symlink_usb);

  while (ps5.isConnected()) {
    ps5.update();

    float x = ps5.axisValue(ps5::AxisID::leftX);
    float y = ps5.axisValue(ps5::AxisID::leftY);

    printf("x: %f y: %f\n", x, y);

    if (ps5.pushed(ps5::ButtonID::option))
      break;

    usleep(5000);
  }

  return 0;
} 
