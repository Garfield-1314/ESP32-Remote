#ifndef ADC_H
#define ADC_H

#include <Arduino.h>

class ADC {
public:
    ADC();  // 构造函数
    void begin();  // 初始化ADC
    void zeroDriftInit(uint16_t sampleCount = 32); // 零漂初始化
    int readRaw(uint8_t pin);  // 读取原始ADC值
    float readVoltage(uint8_t pin);  // 读取电压值

    int readRawFiltered(uint8_t pin);  // 读取滤波后的ADC值
    float readVoltageFiltered(uint8_t pin);  // 读取滤波后的电压值

private:
    static const uint8_t PIN_COUNT = 4;  // 支持的引脚数量
    uint8_t adcPins[PIN_COUNT] = {4, 5, 6, 7};  // ADC引脚数组
    bool validPin(uint8_t pin);  // 验证引脚有效性

    // 零漂相关成员
    int zeroOffset[PIN_COUNT] = {0}; // 零漂偏移

    // 滤波相关成员
    static const uint8_t FILTER_SIZE = 10;  // 滑动平均窗口大小
    int filterBuffer[PIN_COUNT][FILTER_SIZE] = {0};
    uint8_t filterIndex[PIN_COUNT] = {0};
    bool bufferFilled[PIN_COUNT] = {false};
};

extern ADC adc;  // 声明全局ADC对象

#endif