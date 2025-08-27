#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LED {
public:
    LED(uint8_t pin);
    void begin();
    void on();      // 点亮LED（低电平）
    void off();     // 熄灭LED（高电平）
    void toggle();  // 翻转LED状态
    bool isOn();    // 返回LED当前是否点亮
private:
    uint8_t _pin;
    bool _state;
};

#endif
