/************************************* Creating and Deleting Task from Another Task ************************************************************
   Multitasking is accomplished in real-time operating systems by splitting each program into useful tasks. But not all
   functions run during the program's execution. Some tasks are more important and are done more often than others.
   Therefore, in RTOS, when there is no need to call it in the near future or if we no longer need them in the program,
   certain tasks can be deleted during program execution. Thus, we will know how to delete tasks using the FreeRTOS API
   with Arduino throughout this tutorial.

   Commonly Asked Questions:
   1. Why we need to delete Task?
   In RTOS, the program is divided into smaller mini programs also known as tasks or threads. But some tasks are more
   important than other and critical tasks execute more frequency than non-important tasks. Therefore, sometimes we
   delete unnecessary tasks to free memory and create them again when require again.

   2. What Happens when we delete the task?
   The idle task is responsible for releasing the assigned memory of the RTOS kernel from the deleted tasks. It is
   therefore critical that microcontroller processing time does not starve the idle task if your application makes any
   calls to vTaskDelete ().

   3. Does RTOS Kernel automatically delete the resources?
   Only memory allocated to a task by the kernel itself will be freed automatically when the task is deleted.
   Any memory or other resource that the implementation of the task allocated must be freed explicitly.
****************************************** vTaskDelete API ************************************************************
  void vTaskDelete(TaskHandle_t xTask);
  Parameters:
  xTask - The handle of the task to be deleted. Passing NULL will cause the calling task to be deleted.

  It remove a task from the RTOS kernels management. The task being deleted will be removed from all ready, blocked,
  suspended and event lists. With the help of this API function, any task can delete itself and can also delete other
  tasks by passing reference by a handler to vTaskDelete() function.

  NOTE: INCLUDE_vTaskDelete must be defined as 1 for this function to be available.
  VISIT https://www.freertos.org/a00126.html
*********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTaskIdle(void *pvParameters);

TaskHandle_t xTask2Handle;

void setup() {
  Serial.begin(9600);

  // Create Task
  xTaskCreate(vTask1, "TASK1", 100, NULL, 1, NULL);
  xTaskCreate(vTaskIdle, "TASK Idle", 100, NULL, 0, NULL);

  // Start the scheduler to start the tasks executing:
  vTaskStartScheduler();

  // This should never reach void loop():
  while (1);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

void vTask1(void* pvParameters) {
  for (;;)
  {
    Serial.println("Task 1 is running, Task_2 is created");
    xTaskCreate(vTask2, "TASK2", 100, NULL, 2, &xTask2Handle);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void vTask2(void* pvParameters) {
  for (;;)
  {
    Serial.println("Task 2 is runnig and about to delete itself");
    vTaskDelete(xTask2Handle);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void vTaskIdle(void* pvParameters) {
  for (;;)
  {
    Serial.println("Idle task");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
