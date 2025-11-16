#include "gamepad.hpp"

namespace pad {

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

  void ButtonData::update(ButtonEvent event) {
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

  void AxisData::update(AxisEvent event) {
    if (event.id <= input_values_.size())
      input_values_[event.id] = event.value;
  }
}