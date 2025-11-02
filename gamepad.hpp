#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/input.h>
#include <sys/ioctl.h>

#include <string>
#include <memory>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <vector>
#include <type_traits>

namespace pad {

  constexpr float DEFAULT_DEADZONE = 0.05;
  constexpr int DEFAULT_BUTTON_NUM = 20;
  constexpr int DEFALUT_AXIS_NUM = 8;
  constexpr int MAX_EVENTS = 32;

  enum class EventType {
    None, 
    Button, 
    Axis
  };

  struct PadEvent {
    EventType type;
    uint16_t  code;
    int32_t   value;
  };


  /**
   * @brief read event data of game controller 
   * 
   */
  class PadReader {
   private:
    std::string path;

    bool connection_;
    int  fd_{-1};
    input_event raw_event_;
    PadEvent    event_;

    bool openDeviceFile(std::string devfile_path);

   public:
    ~PadReader();

    bool connect(std::string devname);
    void disconnect();
    bool readEvent();

    inline bool isConnected() {
      return this->connection_;
    }

    inline PadEvent getPadEvent() {
      return this->event_;
    }
  };


  struct ButtonEvent {
    uint8_t id;
    bool    state;
  };

  struct AxisEvent {
    uint8_t id;
    float   value;
  };

  // デバイスファイルのCODE->インターフェース用ID の変換テーブルmap
  using code_id_map = std::unordered_map<uint, uint8_t>;

  /**
   * @brief 
   * 
   */
  class PadEventHandler {
   protected:
    code_id_map id_map_;
    PadEvent    event_;
    ButtonEvent button_event_ = {.id = 0, .state = false};
    AxisEvent axis_event_ = {.id = 0, .value = 0.0f};
    float deadzone_{DEFAULT_DEADZONE};

    virtual void handleButtonEvent() = 0;
    virtual void handleAxisEvent() = 0;

   public:
    void handleEvent(PadReader& reader);
    void setDeadZone(float deadzone);

    EventType getEventType() { 
      return this->event_.type; 
    }

    ButtonEvent& getButtonEvent() {
      return this->button_event_;
    }

    AxisEvent& getAxisEvent() {
      return this->axis_event_;
    }    
  };

      /**
   * @brief 
   * 
   * @tparam T 
   */
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
    virtual void update(PadEventHandler& handler) = 0;
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
    void update(PadEventHandler& handler) override;

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
    void update(PadEventHandler& handler) override;

    float getValue(uint8_t id) {
      if (id >= input_values_.size()) {
        return 0.0f;
      }

      return this->input_values_[id];
    }
  };  

  // テンプレートクラスが PadEventHandler を継承している制約
  template<typename Handler, 
    typename = std::enable_if_t<std::is_base_of<PadEventHandler, Handler>::value>>
  class BasePad {
   private:
    std::unique_ptr<PadEventHandler> handler_;
    PadReader   reader_;
    ButtonData  buttons_;
    AxisData    axes_;
    bool is_connected_{false};
    std::string devfile_path_;

   public:
    BasePad(std::string devfile_path, 
            int button_num = DEFAULT_BUTTON_NUM, 
            int axis_num = DEFALUT_AXIS_NUM):
      buttons_(button_num),
      axes_(axis_num)
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
            this->buttons_.update(*(this->handler_));
            break;
          }
          case EventType::Axis: {
            this->axes_.update(*(this->handler_));
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
    GamePad(std::string devfile_path,
            int button_num = DEFAULT_BUTTON_NUM, 
            int axis_num = DEFALUT_AXIS_NUM):
      BasePad<Handler>(devfile_path, button_num, axis_num)
    {
      
    }
  };

}
#endif // GAMEPAD_H