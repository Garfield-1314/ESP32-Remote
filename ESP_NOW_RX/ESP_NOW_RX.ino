#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "mac_receiver.h"  // 保持原有头文件
#include "firstcall.h"
#include "user_rtos.h"     // 包含新的RTOS头文件

MacTransceiver transceiver(1);


void setup() {
  Serial.begin(115200);
  delay(1000);  // 等待串口初始化
  
  FirstCall firstCall(0x01);
  firstCall.begin();
  
  while(1) {
    firstCall.processDiscovery();
    if (firstCall.isConnected()) {
      const uint8_t* peerMac = firstCall.getPeerMac();
      transceiver.begin(peerMac);
      break;
    }
  }
  
  // 初始化RTOS任务
  init_user_rtos();
}

void loop() {
  // 维持连接
  transceiver.updateSender();
  transceiver.updateReceiver();
  vTaskDelay(pdMS_TO_TICKS(10));
}