#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "mac_receiver.h"  // 保持原有头文件
#include "firstcall.h"
#include "user_rtos.h"     // 包含新的RTOS头文件
#include "SBUS.h"
#include "KEY.h"
#include "led.h"
#include "NVS.h"

LED led(3); // 内置LED，ESP32通常为GPIO2

MacTransceiver transceiver(1);
FirstCall firstCall(0x01);
bfs::SbusTx sbus_tx(&Serial, -1, -1, true, false); // RX=默认, TX=默认, 反相，非高速
NVS nvs;

extern KEY myButton;

void setup() {
  sbus_tx.Begin();
  nvs.begin("MAC");
  // Serial.begin(115200); // 用于调试
  led.begin();
  delay(1000);  // 等待串口初始化
  myButton.begin();
  firstCall.begin();

  
  while(1) {
    firstCall.processDiscovery();
    if (firstCall.isConnected()) {
      const uint8_t* peerMac = firstCall.getPeerMac();
      Serial.print("Peer MAC: ");
      for (int i = 0; i < 6; i++) { // 打印MAC地址
        Serial.printf("%02X", peerMac[i]);
        if (i < 5) Serial.print(":");
      }
      transceiver.begin(peerMac);
      break;
    }
  }
  init_user_rtos();
  // 初始化RTOS任务
  
}

void loop() {
  // 维持连接
  transceiver.updateSender();
  transceiver.updateReceiver();
  vTaskDelay(pdMS_TO_TICKS(10));
}
