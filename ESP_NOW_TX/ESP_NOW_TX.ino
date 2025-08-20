#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "mac_sender.h"
#include "ADC.h"
#include "KEY.h"
#include "firstcall.h"
#include "user_rtos.h"  // 包含新的RTOS头文件
#include "SwitchManager.h"

extern MacTransceiver transceiver;
extern SwitchManager switches;

void setup() {
  Serial.begin(115200);
  delay(1000);  // 等待串口初始化
  FirstCall firstCall(0x01);
  firstCall.begin();

  switches.begin();
  // switches.setCallback(handleSwitchChange);

  while(1) {
    firstCall.processDiscovery();
    if (firstCall.isConnected()) {
      const uint8_t* peerMac = firstCall.getPeerMac();
      transceiver.begin(peerMac);
      break;
    }
  }

  adc.begin();
  
  // 初始化RTOS任务
  init_user_rtos();  // 调用RTOS初始化函数
}

// uint8_t switchStates;
void loop() {
  //更新按键
  switches.update();
  // switchStates = switches.getAllStates();

  // Serial.print(switchStates);
  // 维持连接
  transceiver.updateSender();
  transceiver.updateReceiver();
  vTaskDelay(pdMS_TO_TICKS(10));
}