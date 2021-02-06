/************************************ Using the FreeRTOS API from an ISR ***********************************************
  The functionality provided by a FreeRTOS API function from an interrupt service routine (ISR) is always necessary, but
  many FreeRTOS API functions perform acts that are not legitimate within an ISR, the most notable of which places the
  task called the API function in the blocked state. If an API function is called from an ISR, it is not called from a
  task, so no calling task can be put in the Blocked State. This problem is solved by FreeRTOS by providing two versions
  of certain API functions, one version for task use and one version for ISR use. Functions intended for use with ISRs
  have added "FromISR" to their name.

  NOTE: Never call a FreeRTOS API function that does not have “FromISR” in its name from an ISR.
 ******************************** Binary Semaphores Used for Synchronization *******************************************
  The Binary Semaphore API interrupt-safe version can be used to unblock a task any time a specific interrupt occurs,
  synchronizing the task efficiently with the interrupt. This enables the bulk of the processing of disrupted events
  within the coordinated task to be enforced, with only a very fast and short portion remaining directly in the ISR.
 ************************************** SemaphoreCreateBinary API *****************************************************
  SemaphoreHandle_t xSemaphoreCreateBinary(void);
  Returns:
  - If NULL is returned, then the semaphore cannot be created because there is insufficient heap memory available for
   FreeRTOS to allocate the semaphore data structures.
  - A non-NULL value being returned indicates that the semaphore has been created successfully.

  Handles to all the various types of FreeRTOS semaphore are stored in a variable of type SemaphoreHandle_t. Before a
  semaphore can be used, it must be created. To create a binary semaphore, use the xSemaphoreCreateBinary() API function.
 **************************************** xSemaphoreTake API *************************************************************
  BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
  Parameters:
  xSemaphore   - The semaphore being ‘taken’.
  xTicksToWait - The maximum amount of time the task should remain in the Blocked state to wait for the semaphore if it
                is not already available.
  Returns:
  - pdPASS is returned only if the call to xSemaphoreTake() was successful in obtaining the semaphore.
  - pdFALSE, the semaphore is not available.
 **************************************** xSemaphoreGiveFromISR API ***************************************************
  BaseType_t xSemaphoreGiveFromISR( SemaphoreHandle_t xSemaphore,
                                   BaseType_t *pxHigherPriorityTaskWoken );
  Parameters:
  xSemaphore                - The semaphore being ‘given’.
  pxHigherPriorityTaskWoken - If calling xSemaphoreGiveFromISR() causes a task to leave the Blocked state, and the
                             unblocked task has a priority higher than the currently executing task (interrupted),
                             then, internally, xSemaphoreGiveFromISR() will set *pxHigherPriorityTaskWoken to pdTRUE.
  Returns:
  - pdPASS will be returned only if the call to xSemaphoreGiveFromISR() is successful.
  - If a semaphore is already available, it cannot be given, and xSemaphoreGiveFromISR() will return pdFAIL.

  xSemaphoreGiveFromISR() is the interrupt safe version of xSemaphoreGive(), so has the pxHigherPriorityTaskWoken
  parameter.
**********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static void vHandlerTask(void *pvParameters);
static void vPeriodicTask(void *pvParameters);
static uint32_t ulExampleInterruptHandler(void);

SemaphoreHandle_t xBinarySemaphore;

void setup() {
  Serial.begin(9600);

  xBinarySemaphore = xSemaphoreCreateBinary(); // Binary semaphore is created.

  if (xBinarySemaphore != NULL) {
    // Create Tasks when semaphore is created:
    xTaskCreate(vHandlerTask, "Handler", 100, NULL, 3, NULL);
    xTaskCreate(vPeriodicTask, "Periodic", 100, NULL, 1, NULL);

    // Attach interrupt for Arduino digital pin 2:
    pinMode(2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(2), ulExampleInterruptHandler, LOW);

    // Start the scheduler so the created tasks start executing.
    vTaskStartScheduler();
  }

  // This should never reach void loop():
  for (;;);
  return 0;
}

void loop() {}

static void vHandlerTask(void *pvParameters) {
  BaseType_t xStatus;
  for (;;) {
    /* Use the semaphore to wait for the event. The task blocks indefinitely
       meaning this function call will only return once the semaphore has been
       successfully obtained. */
    xStatus = xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

    if (xStatus == pdPASS) {
      Serial.print("Handler task - Processing event.\r\n");
    }
  }
}

static void vPeriodicTask(void *pvParameters) {
  pinMode(LED_BUILTIN, OUTPUT);
  bool LEDstate = false;
  const TickType_t xDelay500ms = pdMS_TO_TICKS(500UL);
  TickType_t xLastWakeTimeTask2;
  xLastWakeTimeTask2 = xTaskGetTickCount();
  for (;;) {
    LEDstate = !(LEDstate);
    digitalWrite(LED_BUILTIN, LEDstate);
    xTaskDelayUntil(&xLastWakeTimeTask2, xDelay500ms);
  }
}

static uint32_t ulExampleInterruptHandler(void) {
  BaseType_t xHigherPriorityTaskWoken;

  // Initialized to pdFALSE as it will get set to pdTRUE inside the interrupt safe API function.
  xHigherPriorityTaskWoken = pdFALSE;

  Serial.print("ulExampleInterruptHandler Task - Interrupt generated.\r\n\r\n\r\n");

  // 'Give' the semaphore to unblock the task:
  xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
}
