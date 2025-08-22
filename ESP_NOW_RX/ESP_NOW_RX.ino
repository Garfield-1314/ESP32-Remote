#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "mac_receiver.h"  // 保持原有头文件
#include "firstcall.h"
#include "user_rtos.h"     // 包含新的RTOS头文件

MacTransceiver transceiver(1);
FirstCall firstCall(0x01);

// void setup() {
//   Serial.setPins(42, 41);
//   Serial.begin(115200);

//   delay(1000);  // 等待串口初始化
  
//   firstCall.begin();
  
//   while(1) {
//     firstCall.processDiscovery();
//     if (firstCall.isConnected()) {
//       const uint8_t* peerMac = firstCall.getPeerMac();
//       transceiver.begin(peerMac);
//       break;
//     }
//   }
  
//   // 初始化RTOS任务
//   init_user_rtos();
// }

// void loop() {
//   // 维持连接
//   transceiver.updateSender();
//   transceiver.updateReceiver();
//   vTaskDelay(pdMS_TO_TICKS(10));
// }


#include "SBUS.h"

bfs::SbusTx sbus_tx(&Serial, -1, -1, true, false); // RX=16, TX=17, 非反相，非高速

void setup() {
//   Serial.begin(115200); // 用于调试
  sbus_tx.Begin();
}

void loop() {
  // 设置要发送的通道数据
    sbus_tx.data().ch[0] = 1000;
    sbus_tx.data().ch[1] = 1500;
    sbus_tx.data().ch[2] = 2000;
    sbus_tx.data().ch[3] = 500;
    sbus_tx.data().ch17 = 0;
    sbus_tx.data().ch18 = 0;
    sbus_tx.data().failsafe = 0;
    sbus_tx.data().lost_frame = 0;

  sbus_tx.Write(); // 发送数据包

  Serial.println("SBUS数据已发送");
  delay(1000); // SBUS帧间隔约7ms，建议10~20ms
}