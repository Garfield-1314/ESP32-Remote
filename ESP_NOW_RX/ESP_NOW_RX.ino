#include "mac_receiver.h"

MacReceiver receiver;

// // 自定义数据处理函数
// void handleReceivedData(uint8_t id, uint16_t dataArray[5], const uint8_t *macAddr) {
//     Serial.print("来自: ");
//     for (int i = 0; i < 6; i++) {
//         Serial.print(macAddr[i], HEX);
//         if (i < 5) Serial.print(":");
//     }
//     Serial.println();
    
//     Serial.print("data1: ");
//     Serial.println(dataArray[0]);
//     Serial.print("data2: ");
//     Serial.println(dataArray[1]);
//     Serial.print("data3: ");
//     Serial.println(dataArray[2]);
//     Serial.print("data4: ");
//     Serial.println(dataArray[3]);
//     // 在这里添加你的自定义处理逻辑
// }

void setup() {
    receiver.begin();
    receiver.setCallback(handleReceivedData);  // 设置自定义回调函数
    
    // 可选：设置超时时间（毫秒）
    receiver.setTimeout(5000);  // 8秒超时
}

void loop() {
    receiver.update();  // 必须定期调用
    
    // 可选：检查连接状态
    if (receiver.isConnected()) {
        // 执行连接状态下的操作
    }
    
    delay(100);
}