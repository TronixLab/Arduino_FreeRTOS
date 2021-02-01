/*********************************** Characteristic of a Task ********************************************************
  A real-time application that uses RTOS can be organized as a series of independent tasks.
  With no coincidental reliance on other tasks inside the system or the RTOS scheduler itself,
  each task runs in it's own framework. At any moment in time, only one task can be performed
  within the program and the real-time RTOS scheduler is responsible for determining which task
  it should be. Therefore, the RTOS scheduler can continuously begin and end each task
  (try to switch each task in or out) as the program executes.
  VISIT https://www.freertos.org/taskandcr.html
  ************************************* Implementing a Task **********************************************************
  A task should have the following structure:
      void vATaskFunction(void *pvParameters) {
        for( ;; )
        {
            -- Task application code here. --
        }
    }
  The vATaskFunction class is defined as a void return function that takes a void pointer as its only
  parameter. This category should be all functions that execute a task. Task characteristics can never
  return, because they are normally implemented as an endless loop.
  VISIT https://www.freertos.org/implementing-a-FreeRTOS-task.html
  *************************************** Task Creation API **********************************************************
  1. TaskHandle_t (type)
  The sort that tasks are referenced by. For example, a call to xTaskCreate returns a TaskHandle_t
  variable (via a pointer parameter) that can then be used to manage the task as a parameter.
  VISIT https://www.freertos.org/a00019.html#xTaskHandle

  2. xTaskCreate()
  BaseType_t xTaskCreate(TaskFunction_t pvTaskCode,
                         const char * const pcName,
                         configSTACK_DEPTH_TYPE usStackDepth,
                         void *pvParameters,
                         UBaseType_t uxPriority,
                         TaskHandle_t *pxCreatedTask);
  Parameters:
  pvTaskCode    - Pointer to the task entry function (name of the function that implements the task).
  pcName        - A descriptive name for the task. This is mainly used to facilitate debugging.
  usStackDepth  - The number of words (not bytes!) to allocate for use as the task’s stack.
                  For example, if the stack is 16-bits wide and usStackDepth is 100, then 200 bytes will be
                  allocated for use as the task’s stack.
  pvParameters  - A value that will passed into the created task as the task’s parameter.
  uxPriority    - The priority at which the created task will execute.
                  Each task is assigned a priority from 0 to (configMAX_PRIORITIES – 1),
                  where configMAX_PRIORITIES is defined within FreeRTOSConfig.h.
  pxCreatedTask - Used to pass a handle to the created task out of the xTaskCreate() function.
                  pxCreatedTask is optional and can be set to NULL.

  Each task needs RAM that is used to be used by the task itself as stack to keep the task state. If a task is
  created using xTaskCreate(), the necessary RAM from the FreeRTOS heap is appropriately assigned. If a task is
  created using xTaskCreateStatic(), the software writer provides the RAM, so that it could be statically allocated
  at the time of compilation.

  NOTE: Create a new task and add it to the list of tasks that are ready to run. configSUPPORT_DYNAMIC_ALLOCATION
        must be set to 1 in FreeRTOSConfig.h
  VISIT https://www.freertos.org/a00125.html
  **************************************** Task Delay API ***********************************************************
  void vTaskDelay(const TickType_t xTicksToDelay);
  Parameters:
  xTicksToDelay - The amount of time, in tick periods, that the calling task should block.

  Delay a task for a given number of ticks. The actual time that the task remains blocked depends on the tick rate.
  The constant portTICK_PERIOD_MS can be used to calculate real time from the tick rate – with the resolution of one
  tick period.

  VTaskDelay() determines the time the task needs to unblock considering the amount of time it calls vTaskDelay().
  Specifying a block time of 100 ticks, for instance, will cause the task to unblock 100 ticks after calling vTaskDelay().
  Thus, vTaskDelay() does not provide a good method of regulating the frequency of a periodic task since the
  direction taken through the code, as well as other tasks and interrupted events, affects the frequency at which
  vTaskDelay() is called and therefore the next time the task is performed.
  VISIT https://www.freertos.org/a00127.html
*********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);
void vTask4(void *pvParameters);
void vTaskIdle(void *pvParameters);

TaskHandle_t xTask1Handle;
TaskHandle_t xTask2Handle;
TaskHandle_t xTask3Handle;
TaskHandle_t xTask4Handle;
TaskHandle_t xTaskIdleHandle;

void setup() {
  Serial.begin(9600);                 // Start serial communication

  xTaskCreate(vTask1,                 // Task function
              "Task1 priority 1",     // Task name
              100,                    // Stack size
              NULL,                   // Parameters passed to the task function
              1,                      // Priority
              &xTask1Handle);         // Task handle

  xTaskCreate(vTask2,                 // Task function
              "Task2 priority 1",     // Task name
              100,                    // Stack size
              NULL,                   // Parameters passed to the task function
              1,                      // Priority
              &xTask2Handle);         // Task handle

  xTaskCreate(vTask3,                 // Task function
              "Task3 priority 2",     // Task name
              100,                    // Stack size
              NULL,                   // Parameters passed to the task function
              2,                      // Priority
              &xTask3Handle);         // Task handle

  xTaskCreate(vTask4,                 // Task function
              "Task4 priority 3",     // Task name
              100,                    // Stack size
              NULL,                   // Parameters passed to the task function
              3,                      // Priority
              &xTask4Handle);         // Task handle

  xTaskCreate(vIdleTask,              // Task function
              "Task Idle",            // Task name
              100,                    // Stack size
              NULL,                   // Parameters passed to the task function
              0,                      // Priority
              &xTaskIdleHandle);      // Task handle

  vTaskStartScheduler();              // Start the scheduler to start the tasks executing

  while (1);                          // This should never reach void loop()
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

static void vTask1(void* pvParameters) {
  for (;;)
  {
    Serial.println(pcTaskGetName(xTask1Handle));
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

static void vTask2(void* pvParameters) {
  for ( ;; )
  {
    Serial.println(pcTaskGetName(xTask2Handle));
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

static void vTask3(void* pvParameters) {
  for ( ;; )
  {
    Serial.println(pcTaskGetName(xTask3Handle));
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

static void vTask4(void* pvParameters) {
  for ( ;; )
  {
    Serial.println(pcTaskGetName(xTask4Handle));
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

static void vIdleTask(void* pvParameters) {
  for ( ;; )
  {
    Serial.println(pcTaskGetName(xTaskIdleHandle));
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
