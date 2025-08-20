#include "mac_receiver.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

MacTransceiver transceiver(1);

uint8_t targetMac[] = {0xE8, 0x06, 0x90, 0x9B, 0x5A, 0x94};

// 任务结构体定义
typedef struct {
  const char* taskName;     // 任务名称
  TaskFunction_t taskFunc;  // 任务函数指针
  uint32_t stackSize;       // 堆栈大小
  UBaseType_t priority;     // 任务优先级
  TaskHandle_t handle;      // 任务句柄
  uint32_t delay;           // 初始延迟(ms)
} rtos_task_t;

// 示例任务函数
void task1(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(1000);  //1hz任务
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    // 精确周期执行
    vTaskDelayUntil(&xLastWake, xFreq);
    Serial.printf("[%s] System uptime: %lu ms\n", taskCfg->taskName, millis()); // 改为英文
  }
}

int rawValue1 = 0;
float voltage1 = 0;
void task2(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(10);  //100hz采样任务
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    // 实际采样操作

    
    // 精确周期等待
    vTaskDelayUntil(&xLastWake, xFreq);
  }
}

void task3(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(5);  //200hz任务
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    // 实际采样操作
    if(transceiver.isSendConnected()) {
        // 发送4个传感器数据
        transceiver.sendData(0, 
                             0,
                             0, 
                             0,
                             0,
                             0,
                             0,
                             0,
                             0,
                             0);
    }
    // 精确周期等待
    vTaskDelayUntil(&xLastWake, xFreq);
  }
}

void task4(void *pvParam) {
  rtos_task_t* taskCfg = (rtos_task_t*)pvParam;
  const TickType_t xFreq = pdMS_TO_TICKS(1000);  //1hz任务
  TickType_t xLastWake = xTaskGetTickCount();
  
  for(;;) {
    // 实际采样操作
    if(transceiver.isReceiveConnected()) {
        transceiver.printReceivedData();
    }
    // 精确周期等待
    vTaskDelayUntil(&xLastWake, xFreq);
  }
}

// 任务配置数组（按优先级排序）
rtos_task_t tasks[] = {
  {"TASK1", task1, 4096, 1, NULL, 500},   // 名称, 函数, 堆栈, 优先级, 句柄, 延迟
  {"TASK2", task2, 4096, 1, NULL, 500},
  {"TASK3", task3, 4096, 1, NULL, 500},
  {"TASK4", task4, 4096, 1, NULL, 500},
};

const uint8_t taskCount = sizeof(tasks) / sizeof(rtos_task_t);

void setup() {
  Serial.begin(115200);
  delay(1000);  // 等待串口初始化
  transceiver.begin(targetMac);
  
  // 创建所有任务
  for (int i = 0; i < taskCount; i++) {
    xTaskCreatePinnedToCore(
      tasks[i].taskFunc,    // 任务函数
      tasks[i].taskName,    // 名称
      tasks[i].stackSize,   // 堆栈大小
      &tasks[i],            // 参数传递
      tasks[i].priority,    // 优先级
      &tasks[i].handle,     // 任务句柄
      tskNO_AFFINITY        // 不指定核心
    );
    
    // 初始延迟
    if (tasks[i].delay > 0) {
      delay(tasks[i].delay);
    }
  }
  
  Serial.println("All tasks started"); // 改为英文
}

void loop() {
  // 维持发送连接
  transceiver.updateSender();
  // 检查接收状态
  transceiver.updateReceiver();
  vTaskDelay(pdMS_TO_TICKS(10));  // 降低空闲任务CPU占用
}