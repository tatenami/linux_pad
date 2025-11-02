#include "gamepad.hpp"
#include <cstdio>
#include <cmath>

namespace pad {
  /**
   * @brief Destroy the Pad Reader
   * 
   */
  PadReader::~PadReader() { disconnect(); }

  /**
   * @brief open device file of target device ( /dev/eventX )
   * 
   */
  bool PadReader::openDeviceFile(std::string devfile_path) {
    // read only, non blocking mode 
    this->fd_ = open(devfile_path.c_str(), O_RDONLY | O_NONBLOCK);

    if (fd_ < 0)
      return false;
    else 
      return true;
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
  bool PadReader::connect(std::string devfile_path) {
    bool is_readable = false;
    connection_ = false;

    is_readable = openDeviceFile(devfile_path);

    connection_ = is_readable;
    event_ = {
      .type =  EventType::None,
      .code =  0,
      .value = 0
    };

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
    this->event_buffer_.resize(MAX_EVENTS);
    this->clearData();
  }

  void ButtonData::clearData() {
    for (auto e: this->input_values_) {
      e = false;
    }
  }

  bool ButtonData::pushed(uint8_t id) {
    if (id >= input_values_.size())
      return false;

    if (event_count_ >= MAX_EVENTS)
      return false;

    if (input_values_[id] == false) {
      return false;
    }

    ButtonEvent event;
    for (int i = 0; i < event_count_; i++) {
      event = event_buffer_[i];
      if (event.id == id && event.state == true) {
        return true;
      }
    }

    return false;
  }

  bool ButtonData::released(uint8_t id) {
    if (id >= input_values_.size())
      return false;

    if (event_count_ >= MAX_EVENTS)
      return false;

    if (input_values_[id] == true) {
      return false;
    }

    ButtonEvent event;
    for (int i = 0; i < event_count_; i++) {
      event = event_buffer_[i];
      if (event.id == id && event.state == false) {
        return true;
      }
    }

    return false;
  }

  void ButtonData::update(PadEventHandler& handler) {
    if (handler.getEventType() != EventType::Button) {
      return;
    }

    ButtonEvent event = handler.getButtonEvent();
    if (event.id < input_values_.size()) 
      input_values_[event.id] = event.state;
    else 
      return;

    if (event_count_ < MAX_EVENTS)
      event_buffer_[event_count_++] = event;    
    else 
      return;

  }

  AxisData::AxisData(uint total_input):
    InputData(total_input) 
  {
    this->clearData();
  }

  void AxisData::clearData() {
    for (auto e: this->input_values_) {
      e = 0.0;
    }
  }

  void AxisData::update(PadEventHandler& handler) {
    if (handler.getEventType() != EventType::Axis) {
      return;
    }

    AxisEvent event = handler.getAxisEvent();
    if (event.id <= input_values_.size())
      input_values_[event.id] = event.value;
  }
}