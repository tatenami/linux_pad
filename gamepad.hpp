#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/input.h>
#include <sys/ioctl.h>

#include <cmath>
#include <vector>
#include <memory>
#include <type_traits>

#include "event_reader.hpp"
#include "event_handler.hpp"

namespace pad {

  template <typename T>
  class InputData {
   protected: 
    std::vector<T> input_values_; 

   public:
    InputData(uint total_input) {
      resize(total_input);
    }

    std::vector<T> getVector() {
      return this->input_values_;
    }

    int getSize() {
      return this->input_values_.size();
    }

    void resize(int total_input) {
      this->input_values_.resize(total_input);
    }

    virtual void clearData() = 0;
  };

  class ButtonData: public InputData<bool> {
   private:
    uint8_t event_count_{0};
    std::vector<ButtonEvent> event_buffer_;

   public:
    ButtonData(uint total_input);
    void clearData() override;
    bool pushed(uint8_t id);
    bool released(uint8_t id);
    void update(ButtonEvent event);

    void clearEvents() {
      event_count_ = 0;
    }

    int getEventCount() {
      return this->event_count_;
    }

    bool getState(uint8_t id) {
      if (id >= this->input_values_.size()) {
        return false;
      }
      return this->input_values_[id];
    }
  };
 
  class AxisData: public InputData<float> {
   public:  
    AxisData(uint total_input);
    void clearData() override;
    void update(AxisEvent event);

    float getValue(uint8_t id) {
      if (id >= input_values_.size()) {
        return 0.0f;
      }

      return this->input_values_[id];
    }
  };  

  namespace concept {
    template <typename ...>
    using void_t = void;

    // T::ButtonCount が存在するかチェック
    template<typename T, typename = void>
    struct has_button_count : std::false_type {};

    template<typename T>
    struct has_button_count<T, void_t<decltype(T::button_count)>> : std::true_type {};

    // T::AxisCount が存在するかチェック
    template<typename T, typename = void>
    struct has_axis_count : std::false_type {};
    
    template<typename T>
    struct has_axis_count<T, void_t<decltype(T::axis_count)>> : std::true_type {};
  }

  // テンプレートクラスが PadEventHandler を継承している制約
  template<typename Handler, 
    typename = std::enable_if_t<std::is_base_of<PadEventHandler, Handler>::value>>
  class BasePad {
    static_assert(concept::has_button_count<Handler>::value, 
      "Handler (T_Handler) must define 'static constexpr size_t button_count'");

    static_assert(concept::has_axis_count<Handler>::value, 
      "Handler (T_Handler) must define 'static constexpr size_t axis_count'");

   private:
    std::unique_ptr<PadEventHandler> handler_;
    EventReader   reader_;
    ButtonData  buttons_;
    AxisData    axes_;
    bool is_connected_{false};
    std::string devfile_path_;

   public:
    BasePad(std::string devfile_path):
      buttons_(Handler::button_count),
      axes_(Handler::axis_count)
    {
      this->devfile_path_ = devfile_path;
      this->handler_ = std::make_unique<Handler>();
      this->is_connected_ = this->reader_.connect(devfile_path);
    }

    ~BasePad() {
      this->reader_.disconnect();
    }

    bool isConnected() {
      return is_connected_;
    }

    bool reconnect() {
      if (this->is_connected_) {
        return false;
      }

      this->reader_.disconnect();
      return this->reader_.connect(this->devfile_path_);
    }
    
    void setDeadZone(float deadzone) {
      this->handler_->setDeadZone(deadzone);
    }

    void resizeInputTotal(int total_button, int total_axis) {
      this->buttons_.resize(total_button);
      this->axes_.resize(total_axis);
    }

    std::vector<bool> getButtonVec() {
      return this->buttons_.getVector();
    }

    std::vector<float> getAxisVec() {
      return this->axes_.getVector();
    }

    void update() {
      if (!(this->reader_.isConnected())) {
        this->is_connected_ = false;
        this->buttons_.clearData();
        this->axes_.clearData();
        return;
      }

      this->buttons_.clearEvents();

      while (this->reader_.readEvent()) {
        this->handler_->handleEvent(this->reader_);
        EventType type = this->handler_->getEventType();

        switch (type) {
          case EventType::Button: {
            ButtonEvent event = (this->handler_)->getButtonEvent();
            this->buttons_.update(event);
            break;
          }
          case EventType::Axis: {
            AxisEvent event = (this->handler_)->getAxisEvent();
            this->axes_.update(event);
            break;
          }
          default:
            break; 
        }
      }
    }

    bool press(uint8_t id) {
      return this->buttons_.getState(id);
    }

    bool pushed(uint8_t id) {
      return this->buttons_.pushed(id);
    }

    bool released(uint8_t id) {
      return this->buttons_.released(id);
    }

    float axisValue(uint8_t id) {
      return this->axes_.getValue(id);
    }
  };

  template <typename Handler,
    typename = std::enable_if_t<std::is_base_of<PadEventHandler, Handler>::value>>
  class GamePad: public BasePad<Handler> {
   public:
    GamePad(std::string devfile_path):
      BasePad<Handler>(devfile_path)
    {
      
    }
  };

  template <typename Handler,
    typename = std::enable_if_t<std::is_base_of<MotionEventHandler, Handler>::value>>
  class BaseMotionSensor {
   private:
    std::unique_ptr<MotionEventHandler> handler_;
    EventReader   reader_;
    AxisData    axes_;
    bool is_connected_{false};
    std::string devfile_path_;

   public:
    BaseMotionSensor(std::string devfile_path, 
            int axis_num = DEFALUT_AXIS_NUM):
      axes_(axis_num)
    {
      this->devfile_path_ = devfile_path;
      this->handler_ = std::make_unique<Handler>();
      this->is_connected_ = this->reader_.connect(devfile_path);
    }

    ~BaseMotionSensor() {
      this->reader_.disconnect();
    }

    bool isConnected() {
      return is_connected_;
    }

    bool reconnect() {
      if (this->is_connected_) {
        return false;
      }

      this->reader_.disconnect();
      return this->reader_.connect(this->devfile_path_);
    }

    void resizeInputTotal(int total_axis) {
      this->axes_.resize(total_axis);
    }

    std::vector<float> getAxisVec() {
      return this->axes_.getVector();
    }

    void update() {
      if (!(this->reader_.isConnected())) {
        this->is_connected_ = false;
        this->axes_.clearData();
        return;
      }

      while (this->reader_.readEvent()) {
        this->handler_->handleEvent(this->reader_);
        AxisEvent event = (this->handler_)->getAxisEvent();
        this->axes_.update(event);
      }
    }

    float axisValue(uint8_t id) {
      return this->axes_.getValue(id);
    }
  };

  template <typename Handler,
    typename = std::enable_if_t<std::is_base_of<MotionEventHandler, Handler>::value>>
  class MotionSensor: public BaseMotionSensor<Handler> {
   public:
    MotionSensor(std::string devfile_path,
            int axis_num = DEFALUT_AXIS_NUM):
      BaseMotionSensor<Handler>(devfile_path, axis_num)
    {
      
    }
  };
}

#endif // GAMEPAD_H