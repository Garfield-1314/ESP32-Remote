#include "ADC.h"

// 实现构造函数
ADC::ADC() {}

// 初始化ADC设置
void ADC::begin() {
    analogReadResolution(11);        // 设置ADC分辨率为11位 (0-2047)
    analogSetAttenuation(ADC_11db);  // 设置衰减为11dB (0-3.3V量程)
    zeroDriftInit(); // 零漂初始化
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
    int pinIdx = -1;
    for (int i = 0; i < PIN_COUNT; i++) {
        if (adcPins[i] == pin) {
            pinIdx = i;
            break;
        }
    }
    if (pinIdx == -1) return -1;
    int value = analogRead(pin) - zeroOffset[pinIdx];
    return value < 0 ? 0 : value;
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
    long sum = 0;
    for (uint8_t i = 0; i < FILTER_SIZE; i++) {
        int value = analogRead(pin) - zeroOffset[pinIdx];
        value = value < 0 ? 0 : value;
        sum += value;
        delay(2); // 可根据实际情况调整采样间隔
    }
    return sum / FILTER_SIZE;
}

// 读取指定引脚的电压值（保留3位小数）
float ADC::readVoltage(uint8_t pin) {
    int rawValue = readRaw(pin);
    if (rawValue < 0) return NAN;  // 无效引脚返回NaN4095
    return rawValue;  // 11位ADC转换公式
}

// 读取指定引脚的滤波电压值
float ADC::readVoltageFiltered(uint8_t pin) {
    int rawValue = readRawFiltered(pin);
    if (rawValue < 0) return NAN;
    return rawValue;
}

// 创建全局实例
ADC adc;

// 零漂初始化实现
void ADC::zeroDriftInit(uint16_t sampleCount) {
    for (int i = 0; i < PIN_COUNT; i++) {
        long sum = 0;
        for (uint16_t s = 0; s < sampleCount; s++) {
            sum += analogRead(adcPins[i]);
            delay(2); // 可根据实际情况调整采样间隔
        }
        zeroOffset[i] = (sum / sampleCount)-1024;
    }
}



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
