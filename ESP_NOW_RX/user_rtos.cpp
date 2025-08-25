#include "user_rtos.h"
#include "mac_receiver.h"  // 包含mac_receiver头文件
#include "SBUS.h"
// 外部对象声明
extern MacTransceiver transceiver;
extern int16_t sbusData[10];
extern bfs::SbusTx sbus_tx;

int datas[] = {0,0,0,0,0,0,0,0,0,0};

// 任务配置数组定义
rtos_task_t tasks[] = {
  // {"TASK1", task1, 4096, 1, NULL, 500},
  {"TASK2", task2, 4096, 1, NULL, 500},
  // {"TASK3", task3, 4096, 1, NULL, 500},
  {"TASK4", task4, 4096, 1, NULL, 500},
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

void task2(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(10);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    // 采样操作预留位置
    // Serial.print(sbusData[0]);
    sbus_tx.data().ch[1] = sbusData[0];
    sbus_tx.data().ch[2] = sbusData[1];
    sbus_tx.data().ch[3] = sbusData[2];
    sbus_tx.data().ch[4] = sbusData[3];

    sbus_tx.data().ch17 = 0;
    sbus_tx.data().ch18 = 0;
    sbus_tx.data().failsafe = 0;
    sbus_tx.data().lost_frame = 0;

    sbus_tx.Write(); // 发送数据包
  }
  vTaskDelayUntil(&xLastWake, xFreq);
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
  const TickType_t xFreq = pdMS_TO_TICKS(10);
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    if(transceiver.isReceiveConnected()) {
        transceiver.printReceivedData();
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