/************************************* vTaskDelayUntil API *********************************************************
  void vTaskDelayUntil(TickType_t *pxPreviousWakeTime,
                     const TickType_t xTimeIncrement);
  Parameters:
  pxPreviousWakeTime - Pointer to a variable that holds the time at which the task was last unblocked. The variable
                     must be initialised with the current time prior to its first use.
  xTimeIncrement     - The cycle time period. The task will be unblocked at time. Calling vTaskDelayUntil with the same
                     xTimeIncrement parameter value will cause the task to execute with a fixed interval period.

  For one important way, this function differs from vTaskDelay(): vTaskDelay() specifies the time that the task wishes
  to unblock relative to the time that vTaskDelay() is called, whilevTaskDelayUntil() specifies the absolute time that
  the task wishes to unblock.

  From the time vTaskDelay() is called, vTaskDelay() will cause a task to block the defined number of ticks. It is also
  difficult to produce a defined execution frequency by using vTaskDelay() on its own, as the period between a task
  unblocking after a vTaskDelay() call and the task next to vTaskDelay() which not be certain (the task may take a
  different path through the code between calls, or each time it is executed, it may be interrupted or preempted a
  different number of times).

  It should be noted that if vTaskDelayUntil() is used to define a wake time that is already in the past, it will return
  instantly (without blocking). Therefore, if the periodic execution is stopped for some purpose (suspended state) that
  causes the task to skip one or more periodic executions, a task using vTaskDelayUntil() to perform periodically would
  have to re-calculate its required wake time.

  This function must not be called while the RTOS scheduler has been suspended by a call to vTaskSuspendAll().

  NOTE: INCLUDE_vTaskDelayUntil must be defined as 1 for this function to be available.
        For Arduino library, the syntax is INCLUDE_xTaskDelayUntil, so the function must written as xTaskDelayUntil()
  VISIT https://www.freertos.org/vtaskdelayuntil.html
*********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);

TaskHandle_t xTask1Handle;
TaskHandle_t xTask2Handle;
TaskHandle_t xTask3Handle;

void setup() {
  Serial.begin(9600);                 // Start serial communication

  xTaskCreate(vTask1,                 // Task function
              "Task1-Period 4Hz",     // Task name
              100,                    // Stack size
              NULL,                   // Parameters passed to the task function
              1,                      // Priority
              &xTask1Handle);         // Task handle

  xTaskCreate(vTask2,                 // Task function
              "Task2-Period 10Hz",    // Task name
              100,                    // Stack size
              NULL,                   // Parameters passed to the task function
              1,                      // Priority
              &xTask2Handle);         // Task handle

  xTaskCreate(vTask3,                 // Task function
              "Task2-Period 1Hz",     // Task name
              100,                    // Stack size
              NULL,                   // Parameters passed to the task function
              1,                      // Priority
              &xTask3Handle);         // Task handle

  vTaskStartScheduler();              // Start the scheduler to start the tasks executing

  while (1);                          // This should never reach void loop()
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

static void vTask1(void* pvParameters) {
  TickType_t xLastWakeTimeTask1;
  const TickType_t xDelay250ms = pdMS_TO_TICKS(250UL);
  xLastWakeTimeTask1 = xTaskGetTickCount();
  for (;;)
  {
    Serial.println(pcTaskGetName(xTask1Handle));
    xTaskDelayUntil(&xLastWakeTimeTask1, xDelay250ms);
  }
}

static void vTask2(void* pvParameters) {
  TickType_t xLastWakeTimeTask2;
  const TickType_t xDelay100ms = pdMS_TO_TICKS(100UL);
  xLastWakeTimeTask2 = xTaskGetTickCount();
  for ( ;; )
  {
    Serial.println(pcTaskGetName(xTask2Handle));
    xTaskDelayUntil(&xLastWakeTimeTask2, xDelay100ms);
  }
}

static void vTask3(void* pvParameters) {
  TickType_t xLastWakeTimeTask3;
  const TickType_t xDelay1000ms = pdMS_TO_TICKS(1000UL);
  xLastWakeTimeTask3 = xTaskGetTickCount();
  for ( ;; )
  {
    Serial.println(pcTaskGetName(xTask3Handle));
    xTaskDelayUntil(&xLastWakeTimeTask3, xDelay1000ms);
  }
}
