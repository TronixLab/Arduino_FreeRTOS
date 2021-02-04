/****************************************** Counting Semaphores ********************************************************
 Just as binary semaphores can be considered to be queues that have a length of one, it is possible to think of 
 counting semaphores as queues that have a length of more than one. The information stored in the queue is not 
 concerned with tasks, only the number of items in the queue. Another space in its queue is used every time a 
 counting semanphore is 'given'. The number of items in the queue is the meaning 'count' of the semaphor.
 *************************************** xSemaphoreCreateCounting ******************************************************
 SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount,
                                            UBaseType_t uxInitialCount);
 Parameters:
 uxMaxCount     - The maximum value to which the semaphore will count. The uxMaxCount value is the length of the queue.
 uxInitialCount - The initial count value of the semaphore after it has been created.

 Returns:
 - If NULL is returned, the semaphore cannot be created because there is insufficient heap memory available for 
   FreeRTOS to allocate the semaphore data structures.
 - A non-NULL value being returned indicates that the semaphore has been created successfully.
***********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static void vHandlerTask(void *pvParameters);
static void vPeriodicTask(void *pvParameters);

static uint32_t ulExampleInterruptHandler(void);

SemaphoreHandle_t xCountingSemaphore;

void setup() {
  Serial.begin(9600);
  xCountingSemaphore = xSemaphoreCreateCounting(10, 1);

  if (xCountingSemaphore != NULL) {
    // Create Tasks when semaphore is created:
    xTaskCreate( vHandlerTask, "Handler", 100, NULL, 3, NULL );
    xTaskCreate( vPeriodicTask, "Periodic", 100, NULL, 1, NULL );

    // Attach interrupt for Arduino digital pin 2:
    pinMode(2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(2), ulExampleInterruptHandler, LOW);

    // Start the scheduler so the created tasks start executing.
    vTaskStartScheduler();
  }
  // This should never reach void loop():
  for ( ;; );
  return 0;
}

void loop() {}

static void vHandlerTask(void *pvParameters) {
  BaseType_t xStatus;
  for (;;)
  {
    /* Use the semaphore to wait for the event. The task blocks indefinitely
       meaning this function call will only return once the semaphore has been
       successfully obtained. */
    xStatus = xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);

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

static uint32_t ulExampleInterruptHandler (void ) {
  BaseType_t xHigherPriorityTaskWoken;

  // Initialized to pdFALSE as it will get set to pdTRUE inside the interrupt safe API function.
  xHigherPriorityTaskWoken = pdFALSE;

  Serial.print("ulExampleInterruptHandler Task - Interrupt generated.\r\n\r\n\r\n");

  // 'Give' the semaphore to unblock the task:
  xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );

  /* following 'gives' are to demonstrate that the semaphore latches the events to
    allow the handler task to process them in turn without events getting lost.*/
  xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );
  xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );
}
