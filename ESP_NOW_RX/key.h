#ifndef KEY_h
#define KEY_h

#include "Arduino.h"

class KEY {
  public:
    // 按键事件类型
    enum EventType {
      NO_EVENT,
      SHORT_PRESS,
      LONG_PRESS
    };
    
    // 构造函数：参数为引脚号，可选参数为短按/长按时间
    KEY(uint8_t pin, unsigned long shortPressTime = 50, unsigned long longPressTime = 800);
    
    // 初始化函数
    void begin();
    
    // 主循环更新函数（必须在loop中定期调用）
    EventType update();
    
    // 设置短按时长阈值
    void setShortPressTime(unsigned long time);
    
    // 设置长按时长阈值
    void setLongPressTime(unsigned long time);
    
    // 复位按键状态
    void reset();
  
  private:
    uint8_t _pin;                     // 按键引脚
    unsigned long _lastReadTime;       // 上次读取时间
    uint8_t _lastState;               // 上一次状态
    bool _pressed;                    // 当前是否按下
    unsigned long _pressStartTime;     // 按下起始时间
    unsigned long _shortPressTime;     // 短按阈值
    unsigned long _longPressTime;      // 长按阈值
};

#endif