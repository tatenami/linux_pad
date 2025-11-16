#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include <linux/input-event-codes.h>

const char *event_type_names[EV_FF_STATUS + 1] = {
  [EV_SYN]       = "EV_SYN",
  [EV_KEY]       = "EV_KEY",
  [EV_REL]       = "EV_REL",
  [EV_ABS]       = "EV_ABS",
  [EV_MSC]       = "EV_MSC",
  [EV_SW]        = "EV_SW",
  // 0x06 から 0x10 までは自動的に NULL で初期化される
  [EV_LED]       = "EV_LED",
  [EV_SND]       = "EV_SND",
  // 0x13 は自動的に NULL で初期化される
  [EV_REP]       = "EV_REP",
  [EV_FF]        = "EV_FF",
  [EV_PWR]       = "EV_PWR",
  [EV_FF_STATUS] = "EV_FF_STATUS",
};

const char *abs_event_names[ABS_TOOL_WIDTH + 1] = {
  [ABS_X]          = "ABS_X",
  [ABS_Y]          = "ABS_Y",
  [ABS_Z]          = "ABS_Z",
  [ABS_RX]         = "ABS_RX",
  [ABS_RY]         = "ABS_RY",
  [ABS_RZ]         = "ABS_RZ",
  [ABS_THROTTLE]   = "ABS_THROTTLE",
  [ABS_RUDDER]     = "ABS_RUDDER",
  [ABS_WHEEL]      = "ABS_WHEEL",
  [ABS_GAS]        = "ABS_GAS",
  [ABS_BRAKE]      = "ABS_BRAKE",
  // 0x0b から 0x0f までは自動的に NULL で初期化される
  [ABS_HAT0X]      = "ABS_HAT0X",
  [ABS_HAT0Y]      = "ABS_HAT0Y",
  [ABS_HAT1X]      = "ABS_HAT1X",
  [ABS_HAT1Y]      = "ABS_HAT1Y",
  [ABS_HAT2X]      = "ABS_HAT2X",
  [ABS_HAT2Y]      = "ABS_HAT2Y",
  [ABS_HAT3X]      = "ABS_HAT3X",
  [ABS_HAT3Y]      = "ABS_HAT3Y",
  [ABS_PRESSURE]   = "ABS_PRESSURE",
  [ABS_DISTANCE]   = "ABS_DISTANCE",
  [ABS_TILT_X]     = "ABS_TILT_X",
  [ABS_TILT_Y]     = "ABS_TILT_Y",
  [ABS_TOOL_WIDTH] = "ABS_TOOL_WIDTH",
};

void get_abs_info(int fd, int code, struct input_absinfo *abs_info) {
  int ret = ioctl(fd, EVIOCGABS(code), abs_info);
  if (ret < 0) {
    perror("[ERROR] Fail to get abs info\n");
  }


}

void print_event_info(int fd, struct input_event *event) {
  printf("TYPE: %s\n", event_type_names[event->type]);
  if (event->type == EV_ABS) {
    struct input_absinfo abs_info;
    get_abs_info(fd, event->code, &abs_info);
    printf("  CODE(ABS): %s\n  VALUE: %d (%d ~ %d)\n", 
      abs_event_names[event->code], event->value,
      abs_info.minimum, abs_info.maximum);
  }
  else {
    // printf("[INFO] CODE: %d VALUE: %d\n", event->code, event->value);
  }
}

int main(int argc, char *argv[]) {
  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    perror("[ERROR] Fail to open file\n");
    return 1;
  }

  struct input_event event;

  while (1) {
    int ret = read(fd, &event, sizeof(event));
    if (ret < 0) {
      perror("[ERROR] Fail to read event\n");
      return 1;
    }

    if (event.type != EV_ABS) continue;

    print_event_info(fd, &event);
  }

  return 0;
}