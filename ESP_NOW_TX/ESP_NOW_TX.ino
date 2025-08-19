#include "mac_sender.h"

// 使用默认引脚和MAC地址
MacSender sender;

void setup() {
    sender.begin();
}

void loop() {
    sender.update();
    
    // 每3秒发送一次示例数据
    static uint32_t lastSend = 0;
    if (millis() - lastSend > 100) {
        uint16_t data1 = random(0,4000);
        uint16_t data2 = random(0,4000);
        uint16_t data3 = random(0,4000);
        uint16_t data4 = random(0,4000);
        
        sender.sendData(data1, data2, data3, data4);
        lastSend = millis();
    }
    
    delay(100);
}