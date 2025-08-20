#ifndef ADC_H
#define ADC_H

#include <Arduino.h>

class ADC {
public:
    ADC();  // 构造函数
    void begin();  // 初始化ADC
    int readRaw(uint8_t pin);  // 读取原始ADC值
    float readVoltage(uint8_t pin);  // 读取电压值

private:
    static const uint8_t PIN_COUNT = 4;  // 支持的引脚数量
    uint8_t adcPins[PIN_COUNT] = {4, 5, 6, 7};  // ADC引脚数组
    bool validPin(uint8_t pin);  // 验证引脚有效性
};

extern ADC adc;  // 声明全局ADC对象

#endif