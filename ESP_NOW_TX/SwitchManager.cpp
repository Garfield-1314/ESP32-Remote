#include "SwitchManager.h"

#define DEBOUNCE_TIME 50  // 消抖时间50ms

SwitchManager::SwitchManager(const uint8_t pins[], uint8_t count) 
    : _count(count) {
    // 动态分配内存
    _pins = new uint8_t[count];
    _currentStates = new bool[count];
    _lastStates = new bool[count];
    _lastChangeTimes = new unsigned long[count];
    
    // 初始化数组
    for(uint8_t i = 0; i < count; i++) {
        _pins[i] = pins[i];
        _currentStates[i] = false;
        _lastStates[i] = false;
        _lastChangeTimes[i] = 0;
    }
    
    _callback = nullptr;  // 初始化回调为空
}

SwitchManager::~SwitchManager() {
    // 释放动态分配的内存
    delete[] _pins;
    delete[] _currentStates;
    delete[] _lastStates;
    delete[] _lastChangeTimes;
}

void SwitchManager::begin() {
    // 配置所有开关引脚为输入模式（硬件上拉）
    for(uint8_t i = 0; i < _count; i++) {
        pinMode(_pins[i], INPUT);
    }
}

void SwitchManager::update() {
    for(uint8_t i = 0; i < _count; i++) {
        // 读取原始状态
        bool rawState = (digitalRead(_pins[i]) == LOW);
        
        // 如果状态发生变化
        if(rawState != _lastStates[i]) {
            _lastStates[i] = rawState;
            _lastChangeTimes[i] = millis();
        }
        
        // 消抖后确定真实状态
        if((millis() - _lastChangeTimes[i]) > DEBOUNCE_TIME) {
            if(_currentStates[i] != _lastStates[i]) {
                _currentStates[i] = _lastStates[i];
                
                // 如果有回调则触发
                if(_callback) {
                    _callback(_pins[i], _currentStates[i]);
                }
            }
        }
    }
}

void SwitchManager::setCallback(SwitchCallback callback) {
    _callback = callback;
}

bool SwitchManager::getState(uint8_t index) const {
    if(index < _count) {
        return _currentStates[index];
    }
    return false;  // 索引无效返回false
}

uint8_t SwitchManager::getAllStates() const {
    uint8_t states = 0;
    for(uint8_t i = 0; i < _count; i++) {
        if(_currentStates[i]) {
            states |= (1 << i);  // 设置对应位
        }
    }
    return states;
}


void handleSwitchChange(uint8_t pin, bool state) {
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" changed to: ");
  Serial.println(state ? "OFF" : "ON");
  
  // 更新所有开关状态并发送
  switch(pin){
    case 15:Serial.print(pin);break;
    case 16:Serial.print(pin);break;
    case 17:Serial.print(pin);break;
    case 18:Serial.print(pin);break;
    case 35:Serial.print(pin);break;
    case 36:Serial.print(pin);break;
    case 37:Serial.print(pin);break;
    case 38:Serial.print(pin);break;
  }
}