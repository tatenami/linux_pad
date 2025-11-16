#ifndef EVNET_READER_H
#define EVNET_READER_H

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <string>

#include "event_types.hpp"


namespace pad {

  class EventReader {
   private:
    std::string path;

    bool connection_;
    int  fd_{-1};
    input_event raw_event_;
    Event    event_;

    bool openDeviceFile(std::string devfile_path);

   public:
    ~EventReader();

    bool connect(std::string devname);
    void disconnect();
    bool readEvent();

    inline bool isConnected() {
      return this->connection_;
    }

    inline Event getEvent() {
      return this->event_;
    }
  };

}

#endif // EVNET_READER_H