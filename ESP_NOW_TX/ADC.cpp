#include "ADC.h"

// 实现构造函数
ADC::ADC() {}

// 初始化ADC设置
void ADC::begin() {
    analogReadResolution(11);        // 设置ADC分辨率为12位 (0-4095)
    analogSetAttenuation(ADC_11db);  // 设置衰减为11dB (0-3.3V量程)
}

// 验证引脚是否在有效范围内
bool ADC::validPin(uint8_t pin) {
    for (int i = 0; i < PIN_COUNT; i++) {
        if (adcPins[i] == pin) {
            return true;
        }
    }
    return false;
}

// 读取指定引脚的原始ADC值
int ADC::readRaw(uint8_t pin) {
    if (!validPin(pin)) return -1;  // 无效引脚返回-1
    return analogRead(pin);
}

// 读取指定引脚的滤波ADC值（滑动平均）
int ADC::readRawFiltered(uint8_t pin) {
    if (!validPin(pin)) return -1;
    int pinIdx = -1;
    for (int i = 0; i < PIN_COUNT; i++) {
        if (adcPins[i] == pin) {
            pinIdx = i;
            break;
        }
    }
    if (pinIdx == -1) return -1;
    int value = analogRead(pin);
    filterBuffer[pinIdx][filterIndex[pinIdx]] = value;
    filterIndex[pinIdx] = (filterIndex[pinIdx] + 1) % FILTER_SIZE;
    if (filterIndex[pinIdx] == 0) bufferFilled[pinIdx] = true;
    int sum = 0;
    uint8_t count = bufferFilled[pinIdx] ? FILTER_SIZE : filterIndex[pinIdx];
    for (uint8_t i = 0; i < count; i++) {
        sum += filterBuffer[pinIdx][i];
    }
    return sum / count;
}

// 读取指定引脚的电压值（保留3位小数）
float ADC::readVoltage(uint8_t pin) {
    int rawValue = readRaw(pin);
    if (rawValue < 0) return NAN;  // 无效引脚返回NaN
    return rawValue * 3.3f / 4095.0f;  // 12位ADC转换公式
}

// 读取指定引脚的滤波电压值
float ADC::readVoltageFiltered(uint8_t pin) {
    int rawValue = readRawFiltered(pin);
    if (rawValue < 0) return NAN;
    return rawValue * 3.3f / 4095.0f;
}

// 创建全局实例
ADC adc;



// void loop() {
//     int rawValue1 = adc.readRaw(4);
//     float voltage1 = adc.readVoltage(4);

//     int rawValue2 = adc.readRaw(5);
//     float voltage2 = adc.readVoltage(5);
    
//     Serial.print(rawValue1);
//     Serial.println(" ");
//     Serial.print(rawValue2);
//     Serial.println(" ");
// }
