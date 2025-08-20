#ifndef SWITCH_MANAGER_H
#define SWITCH_MANAGER_H

#include <Arduino.h>

class SwitchManager {
public:
    typedef void (*SwitchCallback)(uint8_t pin, bool state);

    // 构造函数
    SwitchManager(const uint8_t pins[], uint8_t count);
    
    // 析构函数
    ~SwitchManager();
    
    // 初始化
    void begin();
    
    // 更新开关状态
    void update();
    
    // 设置回调函数
    void setCallback(SwitchCallback callback);
    
    // 获取开关状态
    bool getState(uint8_t index) const;
    
    // 获取所有开关状态
    uint8_t getAllStates() const;

private:
    uint8_t* _pins;
    uint8_t _count;
    bool* _currentStates;
    bool* _lastStates;
    unsigned long* _lastChangeTimes;
    SwitchCallback _callback;
    
    bool checkDebounce(uint8_t index);
};

void handleSwitchChange(uint8_t pin, bool state);

#endif