#include "padbase.hpp"
#include <cstdio>
#include <cmath>

namespace {
  const std::string devlist_path = "/proc/bus/input/devices";
}

namespace pad {
  /**
   * @brief Destroy the Pad Reader
   * 
   */
  PadReader::~PadReader() { disconnect(); }

  /**
   * @brief search target device-name in `/proc/bus/input/devices`
   * 
   * @param devname target device name in `/proc/bus/input/devices`
   * @param stream ifstream of `/proc/bus/input/devices`
   */
  void PadReader::searchDeviceName(std::string devname, std::ifstream& stream) {
    bool is_found = false;
    std::string buf;

    while (std::getline(stream, buf)) {
      if (buf.find(devname) != std::string::npos) {
        is_found = true;
        break;
      }
    }

    if (!is_found) {
      throw std::string("Faild to find device name");
    }
  }

  /**
   * @brief search device file of target device in `/proc/bus/input/devices`
   * 
   * @param stream ifstream of `/proc/bus/input/devices`
   */
  void PadReader::searchDeviceFile(std::ifstream& stream) {
    std::string buf;
    std::regex  regex_devfile(R"(event(\d+))");
    std::smatch smatch;

    // search device file name
    while (std::getline(stream, buf)) {
      if (buf.find("Handlers=") != std::string::npos) break;
    }

    // select device file [ event? ]
    if (std::regex_search(buf, smatch, regex_devfile)) {
      this->path += smatch[0].str();
    }
    else {
      throw std::string("Failed find device file");
    }
  }

  /**
   * @brief open device file of target device ( /dev/eventX )
   * 
   */
  void PadReader::openDeviceFile() {
    // read only, non blocking mode 
    this->fd_ = open(path.c_str(), O_RDONLY | O_NONBLOCK);

    if (this->fd_ == -1) {
      throw std::string("Faild to open device file");
    }
  }

  /**
   * @brief close `fd` of device file 
   * 
   */
  void PadReader::disconnect() { close(this->fd_); }

  /**
   * @brief prepare reading device file of `devname`
   * 
   * @param devname target device name in `/proc/bus/input/devices`
   * @retval `true`: succeed in opening device file and ready to read
   * @retval `false`: fail to searching or opening device file
   */
  bool PadReader::connect(std::string devname) {
    std::ifstream read_stream(devlist_path);
    bool is_readable = false;
    connection_ = false;

    try {
      searchDeviceName(devname, read_stream);
      searchDeviceFile(read_stream);
      openDeviceFile();
      is_readable = true; 
    }
    catch (std::string msg) {
      std::printf("[ERROR] %s\n", msg.c_str());
      return false;
    }

    connection_ = is_readable;
    event_ = {
      .type =  EventType::None,
      .code =  0,
      .value = 0
    };

    read_stream.close();
    return is_readable;
  }

  /**
   * @brief read raw-event from device file
   * 
   * @retval true: read raw-event 
   * @retval false: no raw-event or fail to read  
   */
  bool PadReader::readEvent() {
    if (read(this->fd_, &(raw_event_), sizeof(raw_event_)) > 0) {

      if (raw_event_.type == EV_SYN) {
        return false;
      }

      switch (raw_event_.type) {
        case EV_ABS: event_.type = EventType::Axis;   break;
        case EV_KEY: event_.type = EventType::Button; break;
      }

      event_.code  = raw_event_.code;
      event_.value = raw_event_.value; 

      return true;
    }
    else {
      // 再読込エラーでなければ，ノンブロッキングread以外のエラーなので接続終了
      if (errno != EAGAIN)
        connection_ = false;

      return false;
    }   
  }

  void PadEventHandler::addCodeIdEntry(uint event_code, uint8_t ui_id) {
    this->id_map_[event_code] = ui_id;
  }

  void PadEventHandler::setDeadZone(float deadzone) {
    this->deadzone_ = deadzone;
  }

  void PadEventHandler::handleEvent(PadReader& reader) {
    this->event_ = reader.getPadEvent();

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


  /* [ InputData member functions ] */

  ButtonData::ButtonData(uint total_input):
    InputData(total_input)
  {
    this->type_ = EventType::Button;
    this->clear();
  }

  void ButtonData::clear() {
    for (auto e: this->list_) {
      e = false;
    }
  }

  void ButtonData::update(PadEventHandler& handler) {
    if (handler.getEventType() == EventType::Button) {
      update_flag_ = true;
      event_ = handler.getButtonEvent();
      if (event_.id <= this->size_)
        list_[event_.id] = event_.state;
    }
  }

  AxisData::AxisData(uint total_input):
    InputData(total_input) 
  {
    this->type_ = EventType::Axis;
    this->clear();
  }

  void AxisData::clear() {
    for (auto e: this->list_) {
      e = 0.0;
    }
  }

  void AxisData::update(PadEventHandler& handler) {
    if (handler.getEventType() == EventType::Axis) {
      event_ = handler.getAxisEvent();
      if (event_.id <= this->size_) {
        list_[event_.id] = event_.value;
      }
    }
  }
}