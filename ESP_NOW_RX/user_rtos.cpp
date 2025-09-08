#include "user_rtos.h"
#include "mac_receiver.h"  // 包含mac_receiver头文件
#include "SBUS.h"
#include "KEY.h"
// 外部对象声明
extern MacTransceiver transceiver;
extern int16_t sbusData[11];
extern bfs::SbusTx sbus_tx;

KEY myButton(39);

int datas[] = {0,0,0,0,0,0,0,0,0,0};

// 任务配置数组定义
rtos_task_t tasks[] = {
  // {"TASK1", task1, 4096, 1, NULL, 500},
  {"TASK2", task2, 4096, 1, NULL, 500},
  // {"TASK3", task3, 4096, 1, NULL, 500},
  {"TASK4", task4, 4096, 1, NULL, 500},
  {"TASK5", task5, 4096/2, 1, NULL, 500},
};

const uint8_t taskCount = sizeof(tasks) / sizeof(rtos_task_t);

// 任务函数实现
void task1(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(1000);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    vTaskDelayUntil(&xLastWake, xFreq);
    Serial.printf("[%s] System uptime: %lu ms\n", taskCfg->taskName, millis());
  }
}

extern uint8_t flap;

void task2(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(10);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    // 采样操作预留位置
    // Serial.print(sbusData[0]);
    if(flap == 1){
        sbusData[0] = 1000;
        sbusData[1] = 1000;
        sbusData[2] = 1000;
        sbusData[3] = 1000;

        sbusData[4] = 1000;
        sbusData[5] = 1000;
        sbusData[6] = 1000;
        sbusData[7] = 1000;
        sbusData[8] = 2000;
        sbusData[9] = 1000;
        sbusData[10] = 1000;
        sbusData[11] = 1000;
    }
    sbus_tx.data().ch[0] = sbusData[0];
    sbus_tx.data().ch[1] = sbusData[1];
    sbus_tx.data().ch[2] = sbusData[2];
    sbus_tx.data().ch[3] = sbusData[3];

    sbus_tx.data().ch[4] = sbusData[4];
    sbus_tx.data().ch[5] = sbusData[5];
    sbus_tx.data().ch[6] = sbusData[6];
    sbus_tx.data().ch[7] = sbusData[7];
    sbus_tx.data().ch[8] = sbusData[8];
    sbus_tx.data().ch[9] = sbusData[9];
    sbus_tx.data().ch[10] = sbusData[10];
    sbus_tx.data().ch[11] = sbusData[11];

    sbus_tx.data().ch17 = 0;
    sbus_tx.data().ch18 = 0;
    sbus_tx.data().failsafe = 0;
    sbus_tx.data().lost_frame = 0;
    // sbus_tx.Write(); // 发送数据包
    if(flap == 0){
      sbus_tx.Write(); // 发送数据包
      // Serial.println("Flap!");
    }
    vTaskDelayUntil(&xLastWake, xFreq);
  }
}

void task3(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(5);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    if(transceiver.isSendConnected()) {
        transceiver.sendData(datas[0], datas[1], datas[2], datas[3], datas[4], datas[5], datas[6], datas[7], datas[8], datas[9], datas[10]);
    }
    vTaskDelayUntil(&xLastWake, xFreq);
  }
}

void task4(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(10);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    if(transceiver.isReceiveConnected()) {
        transceiver.printReceivedData();
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
        delay(1000);
        ESP.restart();
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