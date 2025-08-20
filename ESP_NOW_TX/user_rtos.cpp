#include "user_rtos.h"
#include "mac_sender.h"
#include "ADC.h"
#include "KEY.h"
#include <WiFi.h>  // 用于MAC地址获取
#include "SwitchManager.h"

// 对象声明
MacTransceiver transceiver(1);
KEY myButton(39);
// 开关引脚定义
const uint8_t switchPins[] = {15, 16, 17, 18, 35, 36, 37, 38};
const uint8_t switchCount = sizeof(switchPins) / sizeof(switchPins[0]);

// 创建开关管理器实例
SwitchManager switches(switchPins, switchCount);

int datas[] = {0,0,0,0,0,0,0,0,0,0};

// 任务配置数组定义
rtos_task_t tasks[] = {
  {"TASK1", task1, 4096, 1, NULL, 500},
  {"TASK2", task2, 4096, 1, NULL, 500},
  {"TASK3", task3, 4096, 1, NULL, 500},
  {"TASK4", task4, 4096, 1, NULL, 500},
  {"TASK5", task5, 2048, 1, NULL, 500},
};

const uint8_t taskCount = sizeof(tasks) / sizeof(rtos_task_t);

// 任务函数实现
void task1(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(1000);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    vTaskDelayUntil(&xLastWake, xFreq);
    // Serial.printf("[%s] System uptime: %lu ms\n", taskCfg->taskName, millis());
  }
}

void task2(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(10);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    datas[0] = adc.readRaw(4);
    datas[1] = adc.readVoltage(4);
    vTaskDelayUntil(&xLastWake, xFreq);
  }
}

void task3(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(5);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    if(transceiver.isSendConnected()) {
        transceiver.sendData(datas[0], datas[1], datas[2], datas[3], datas[4], datas[5], datas[6], datas[7], datas[8], datas[9]);
    }
    vTaskDelayUntil(&xLastWake, xFreq);
  }
}

void task4(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(1000);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    if(transceiver.isReceiveConnected()) {
        // transceiver.printReceivedData();
    }
    vTaskDelayUntil(&xLastWake, xFreq);
  }
}

void task5(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(10);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    KEY::EventType event = myButton.update();
  
    switch(event) {
      case KEY::SHORT_PRESS:
        Serial.println("Short Press Detected");
        break;
        
      case KEY::LONG_PRESS:
        Serial.print("Local MAC: ");
        Serial.println(WiFi.macAddress());
        break;
        
      case KEY::NO_EVENT:
        // No action
        break;
    }
    vTaskDelayUntil(&xLastWake, xFreq);
  }
}

// RTOS初始化函数
void init_user_rtos() {
  // 创建所有任务
  for (int i = 0; i < taskCount; i++) {
    xTaskCreatePinnedToCore(
      tasks[i].taskFunc,
      tasks[i].taskName,
      tasks[i].stackSize,
      &tasks[i],
      tasks[i].priority,
      &tasks[i].handle,
      tskNO_AFFINITY
    );
    
    // 初始延迟
    if (tasks[i].delay > 0) {
      delay(tasks[i].delay);
    }
  }
  
  Serial.println("All tasks started");
}