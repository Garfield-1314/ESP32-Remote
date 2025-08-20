#include "KEY.h"

KEY::KEY(uint8_t pin, unsigned long shortPressTime, unsigned long longPressTime) :
  _pin(pin),
  _shortPressTime(shortPressTime),
  _longPressTime(longPressTime)
{
  // 初始化成员变量
  _lastState = HIGH;
  _pressed = false;
  _pressStartTime = 0;
  _lastReadTime = 0;
}

void KEY::begin() {
  pinMode(_pin, INPUT);  // 外部上拉模式
}

KEY::EventType KEY::update() {
  unsigned long currentTime = millis();
  
  // 10ms采样周期（节省CPU资源）
  if(currentTime - _lastReadTime < 10) {
    return NO_EVENT;
  }
  _lastReadTime = currentTime;
  
  // 读取当前状态（外部上拉：HIGH=未按下，LOW=按下）
  uint8_t currentState = digitalRead(_pin);
  
  // 状态变化检测
  if(currentState != _lastState) {
    _lastState = currentState;
    
    // 按下事件 (下降沿)
    if(currentState == LOW) {
      _pressed = true;
      _pressStartTime = currentTime;
      return NO_EVENT;
    }
    // 释放事件 (上升沿)
    else {
      _pressed = false;
      
      // 计算按下持续时间
      unsigned long pressDuration = currentTime - _pressStartTime;
      
      // 短按检测
      if(pressDuration >= _shortPressTime && pressDuration < _longPressTime) {
        return SHORT_PRESS;
      }
    }
  }
  // 长按检测（保持按下状态）
  else if(_pressed && (currentTime - _pressStartTime >= _longPressTime)) {
    _pressed = false; // 长按只触发一次
    return LONG_PRESS;
  }
  
  return NO_EVENT;
}

void KEY::setShortPressTime(unsigned long time) {
  _shortPressTime = time;
}

void KEY::setLongPressTime(unsigned long time) {
  _longPressTime = time;
}

void KEY::reset() {
  _pressed = false;
  _lastState = digitalRead(_pin); // 更新到当前状态
}



// #include "KEY.h"

// // 创建按键实例(GPIO39, 短按50ms, 长按800ms)
// KEY myButton(39);

// void setup() {
//   Serial.begin(115200);
//   myButton.begin();
//   Serial.println("Button Library Demo");
// }

// void loop() {
//   // 更新按键状态
//   KEY::EventType event = myButton.update();
  
//   switch(event) {
//     case KEY::SHORT_PRESS:
//       Serial.println("Short Press Detected");
//       // 添加短按功能代码...
//       break;
      
//     case KEY::LONG_PRESS:
//       Serial.println("Long Press Detected");
//       // 添加长按功能代码...
//       break;
      
//     case KEY::NO_EVENT:
//       // 无按键事件  
//       break;
//   }
  
//   // 此处可添加其他任务
//   delay(1); // 降低CPU使用率
// }