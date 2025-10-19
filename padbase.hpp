#ifndef PAD_BASE_H
#define PAD_BASE_H

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

  const float default_deadzone = 0.05;
  const int default_button_num = 20;
  const int default_axis_num = 8;

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
    float deadzone_{default_deadzone};

    virtual void handleButtonEvent() = 0;
    virtual void handleAxisEvent() = 0;

   public:
    void handleEvent(PadReader& reader);
    void setDeadZone(float deadzone);

    inline EventType getEventType() { 
      return this->event_.type; 
    }

    inline ButtonEvent& getButtonEvent() {
      return this->button_event_;
    }

    inline AxisEvent& getAxisEvent() {
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
    uint size_;
    EventType type_;
    std::vector<T> list_; 

   public:
    InputData(uint total_input) {
      this->list_.resize(total_input);
      this->size_ = list_.size();
    }

    std::vector<T> getVector() {
      return this->list_;
    }

    int getSize() {
      return this->list_.size();
    }

    void resize(int total_input) {
      this->list_.resize(total_input);
    }

    virtual void clear() = 0;
    virtual void update(PadEventHandler& handler) = 0;
  };

  class ButtonData: public InputData<bool> {
   protected:
    bool update_flag_{false};
    ButtonEvent event_{0, false};

   public:
    ButtonData(uint total_input);
    void clear() override;
    void update(PadEventHandler& handler) override;

    inline bool IDUpdated(uint8_t id) {
      return update_flag_ && (event_.id == id);
    }

    inline void resetUpdateFlag() {
      this->update_flag_ = false;
    }

    inline bool getState(uint8_t id) {
      return this->list_.at(id);
    }
  };
 
  class AxisData: public InputData<float> {
   protected:
    AxisEvent event_{0, 0.0};

   public:  
    AxisData(uint total_input);
    void clear() override;
    void update(PadEventHandler& handler) override;

    inline float getValue(uint8_t id) {
      return this->list_.at(id);
    }
  };  

  // テンプレートクラスが PadEventHandler を継承している制約
  template<typename Handler, 
    typename = std::enable_if_t<std::is_base_of<PadEventHandler, Handler>::value>>
  class BasePad {
   protected:
    std::unique_ptr<PadEventHandler> handler_;
    PadReader   reader_;
    ButtonData  buttons_;
    AxisData    axes_;
    bool is_connected_{false};
    std::string devfile_path_;

   public:
    BasePad(std::string devfile_path, 
            int button_num = default_button_num, 
            int axis_num = default_axis_num):
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

    inline std::vector<bool> getButtonVec() {
      return this->buttons_.getVector();
    }

    inline std::vector<float> getAxisVec() {
      return this->axes_.getVector();
    }

    void update() {
      if (!(this->reader_.isConnected())) {
        this->is_connected_ = false;
        this->buttons_.clear();
        this->axes_.clear();
        return;
      }

      if (!this->reader_.readEvent()) {
        return;
      }

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
  };

}
#endif // PAD_BASE_H