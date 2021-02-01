/*************************************** vTaskPrioritySet API **********************************************************
  void vTaskPrioritySet(TaskHandle_t xTask,
                        UBaseType_t uxNewPriority);
  Parameters:
  xTask         - Handle to the task for which the priority is being set. Passing a NULL handle results in the priority
                  of the calling task being set.
  uxNewPriority - The priority to which the task will be set.

  It can set the priority of any task. A context switch will occur before the function returns if the priority being
  set is higher than the currently executing task.

  NOTE: Before using this API, make sure that INCLUDE_vTaskPrioritySet is set to 1 in FreeRTOSConfig.h header file.
  VISIT https://www.freertos.org/a00129.html
**********************************************************************************************************************/

#include "Arduino_FreeRTOS.h"
#include "task.h"

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

TaskHandle_t xTask1Handle;
TaskHandle_t xTask2Handle;

void setup() {
  Serial.begin(9600);

  xTaskCreate(vTask1, "TASK1", 100, NULL, 3, &xTask1Handle);
  xTaskCreate(vTask2, "TASK2", 100, NULL, 2, &xTask2Handle);

  vTaskStartScheduler();              // Start the scheduler to start the tasks executing

  while (1);                          // This should never reach void loop()
  return 0;
}

void loop()  {
  // Empty. Things are done in Tasks.
}

void vTask1(void* pvParameters) {
  for (;;)
  {
    int randPriority = random(0, 4);
    Serial.print("Task1 Priority: ");
    Serial.println(randPriority);
    vTaskPrioritySet(xTask1Handle, randPriority);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void vTask2(void* pvParameters) {
  for (;;)
  {
    int randPriority = random(0, 4);
    Serial.print("Task2 Priority: ");
    Serial.println(randPriority);
    vTaskPrioritySet(xTask2Handle, randPriority);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
