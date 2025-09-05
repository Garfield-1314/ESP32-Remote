#include "mac_sender.h"
#include "ADC.h"
#include "KEY.h"
#include "firstcall.h"
#include "user_rtos.h"  // 包含新的RTOS头文件
#include "SwitchManager.h"
#include "NVS.h"
#include "led.h"

extern MacTransceiver transceiver;
extern SwitchManager switches;

NVS nvs;
FirstCall firstCall(0x01);
LED led(3);

extern KEY myButton;

void setup() {
  Serial.begin(115200);
  nvs.begin("MAC");
  led.begin();

  delay(1000);  // 等待串口初始化
  myButton.begin();

  firstCall.begin();
  adc.begin();
  adc.zeroDriftInit(20);

  switches.begin();

  init_user_rtos();  // 调用RTOS初始化函数

  while(1) {
    if (nvs.getInt("MAC0",0)==0){
      firstCall.processDiscovery();
      if (firstCall.isConnected()) {
        const uint8_t* peerMac = firstCall.getPeerMac();
        transceiver.begin(peerMac);
        break;
      }
    }
    else{
      uint8_t MAC[6];
      MAC[0] = nvs.getInt("MAC0",0);
      MAC[1] = nvs.getInt("MAC1",0);
      MAC[2] = nvs.getInt("MAC2",0);
      MAC[3] = nvs.getInt("MAC3",0);
      MAC[4] = nvs.getInt("MAC4",0);
      MAC[5] = nvs.getInt("MAC5",0);

      transceiver.begin(MAC);
      Serial.printf("[NVC]: %02X:%02X:%02X:%02X:%02X:%02X\n",
              MAC[0], MAC[1], MAC[2], 
              MAC[3], MAC[4], MAC[5]);
      break;
    }
  }
}

// uint8_t switchStates;
void loop() {
  //更新按键
  switches.update();

  // 维持连接
  transceiver.updateSender();
  transceiver.updateReceiver();
  vTaskDelay(pdMS_TO_TICKS(10));
}