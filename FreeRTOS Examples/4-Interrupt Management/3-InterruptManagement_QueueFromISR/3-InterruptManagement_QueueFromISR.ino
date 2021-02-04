/********************************** Using Queues within an Interrupt Service Routine ************************************
  Binary and counting semaphores are used to communicate events. Queues are used to communicate events, and to
  transfer data.

  xQueueSendToFrontFromISR() is the version of xQueueSendToFront() that is safe to use in an interrupt service
  routine, xQueueSendToBackFromISR() is the version of xQueueSendToBack() that is safe to use in an interrupt service
  routine, and xQueueReceiveFromISR() is the version of xQueueReceive() that is safe to use in an interrupt service
  routine.
 ***************************** xQueueSendToFrontFromISR and xQueueSendToBackFromISR API *********************************
  BaseType_t xQueueSendToFrontFromISR(QueueHandle_t xQueue,
                                      void *pvItemToQueue BaseType_t *pxHigherPriorityTaskWoken);

  BaseType_t xQueueSendToBackFromISR(QueueHandle_t xQueue, 
                                     void *pvItemToQueue BaseType_t *pxHigherPriorityTaskWoken);
  Parameters:
  xQueue                    - The handle of the queue to which the data is being sent (written). The queue handle 
                              will have been returned from the call to xQueueCreate() used to create the queue.
  pvItemToQueue             - pointer to the data that will be copied into the queue.
  pxHigherPriorityTaskWoken - If xQueueSendToFrontFromISR() or xQueueSendToBackFromISR() sets this value to pdTRUE, 
                              then a context switch should be performed before the interrupt is exited. This will 
                              ensure that the interrupt returns directly to the highest priority Ready state task.
  Returns:
  - pdPASS is returned only if data has been sent successfully to the queue.
  - errQUEUE_FULL is returned if data cannot be sent to the queue because the queue is already full.
**********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "queue.h"

static void vIntegerGenerator( void *pvParameters );
static void vStringPrinter( void *pvParameters );

static uint32_t ulExampleInterruptHandler( void );

QueueHandle_t xIntegerQueue, xStringQueue;

void setup() {
  Serial.begin(9600);

  /*Before a queue can be used it must first be created.  Create both queues
    used by this example.  One queue can hold variables of type uint32_t,
    the other queue can hold variables of type char*.  Both queues can hold a
    maximum of 10 items.  A real application should check the return values to
    ensure the queues have been successfully created.*/
  xIntegerQueue = xQueueCreate(10, sizeof(uint32_t));
  xStringQueue = xQueueCreate(10, sizeof(char *));

  // Create the task that uses a queue to pass integers to the interrupt service routine.
  xTaskCreate(vIntegerGenerator, "IntGen", 100, NULL, 1, NULL);

  // Create the task that prints out the strings sent to it from the interrupt service routine.
  xTaskCreate(vStringPrinter, "String", 100, NULL, 2, NULL);

  // Attach interrupt for Arduino digital pin 2:
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), ulExampleInterruptHandler, LOW);

  // Start the scheduler so the created tasks start executing.
  vTaskStartScheduler();

  // This should never reach void loop.
  for (;;);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

static void vQueuePrinter(void *pvParameters) {
  volatile int valueFromQueue = 0;
  for (;;)
  {
    // Block on the queue to wait for data to arrive.
    xQueueReceive(xStringQueue, &valueFromQueue, portMAX_DELAY );
    Serial.println(valueFromQueue);
  }
}

static void vIntegerGenerator(void *pvParameters) {
  TickType_t xLastExecutionTime;
  const TickType_t xDelay200ms = pdMS_TO_TICKS(200UL), xDontBlock = 0;
  uint32_t ulValueToSend = 0;
  BaseType_t i;

  // Initialize the variable used by the call to vTaskDelayUntil().
  xLastExecutionTime = xTaskGetTickCount();
  for (;;)
  {
    xTaskDelayUntil(&xLastExecutionTime, xDelay200ms);

    /* Send five numbers to the queue, each value one higher than the
      previous value.  The numbers are read from the queue by the interrupt
      service routine.  The interrupt service routine always empties the
      queue, so this task is guaranteed to be able to write all five values
      without needing to specify a block time. */
    for (i = 0; i < 5; i++)
    {
      xQueueSendToBack( xIntegerQueue, &ulValueToSend, xDontBlock );
      ulValueToSend++;
    }
  }
}

static void vStringPrinter(void *pvParameters) {
  char *pcString;
  for (;;)
  {
    /* Block on the queue to wait for data to arrive. */
    xQueueReceive(xStringQueue, &pcString, portMAX_DELAY);

    /* Print out the received string. */
    Serial.println(pcString);
  }
}

static uint32_t ulExampleInterruptHandler(void) {
  BaseType_t xHigherPriorityTaskWoken;
  uint32_t ulReceivedNumber;

  /*The strings are declared static const to ensure they are not allocated on the
    interrupt service routine's stack, and exist even when the interrupt service
    routine is not executing. */
  static const char *pcStrings[] =
  {
    "String 0\r\n",
    "String 1\r\n",
    "String 2\r\n",
    "String 3\r\n"
  };

  /* As always, xHigherPriorityTaskWoken is initialized to pdFALSE to be able
    to detect it getting set to pdTRUE inside an interrupt safe API function. */
  xHigherPriorityTaskWoken = pdFALSE;

  /* Read from the queue until the queue is empty. */
  while (xQueueReceiveFromISR(xIntegerQueue, &ulReceivedNumber, &xHigherPriorityTaskWoken) != errQUEUE_EMPTY)
  {
    /* Truncate the received value to the last two bits (values 0 to 3
      inc.), then use the truncated value as an index into the pcStrings[]
      array to select a string (char *) to send on the other queue. */
    ulReceivedNumber &= 0x03;
    xQueueSendToBackFromISR(xStringQueue, &pcStrings[ulReceivedNumber], &xHigherPriorityTaskWoken);
  }
  Serial.print("Generator task - Interrupt generated.\r\n\r\n\r\n");
}
