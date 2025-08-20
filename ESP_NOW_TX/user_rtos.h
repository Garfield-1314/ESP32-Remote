#ifndef USER_RTOS_H
#define USER_RTOS_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// 任务结构体定义
typedef struct {
  const char* taskName;     // 任务名称
  TaskFunction_t taskFunc;  // 任务函数指针
  uint32_t stackSize;       // 堆栈大小
  UBaseType_t priority;     // 任务优先级
  TaskHandle_t handle;      // 任务句柄
  uint32_t delay;           // 初始延迟(ms)
} rtos_task_t;

// 任务函数声明
void task1(void *pvParam);
void task2(void *pvParam);
void task3(void *pvParam);
void task4(void *pvParam);
void task5(void *pvParam);

// 外部变量声明
extern rtos_task_t tasks[];
extern const uint8_t taskCount;

// RTOS初始化函数
void init_user_rtos();

#endif // USER_RTOS_H