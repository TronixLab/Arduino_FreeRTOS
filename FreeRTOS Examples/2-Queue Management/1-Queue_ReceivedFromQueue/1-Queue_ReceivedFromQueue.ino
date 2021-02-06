/**************************************** Characteristics of a Queue ***************************************************
  A queue can hold a finite number of data items of a fixed size. Its length is called the maximum number of elements
  that a queue can carry. When constructing the queue, both the length and the size of each data item are set. Queues
  are usually used as First In First Out (FIFO) buffers, where data is written to the queue's end (tail) and extracted
  from the queue's front (head).

  Access by Multiple Task:
  In their own right, queues are objects that can be accessed by any task or ISR that knows their presence. You can
  write any number of tasks in the same queue, and you can read any number of tasks in the same queue. In practice, it
  is very common to have multiple writers in a queue, but far less common to have multiple readers in a queue.

  Blocking on Queue Reads:
  When a task attempts to read from a queue, it can optionally specify a ‘block’ time. This is the time the task will
  be kept in the Blocked state to wait for data to be available from the queue, should the queue already be empty. A
  task that is in the Blocked state, waiting for data to become available from a queue, is automatically moved to the
  Ready state when another task or interrupt places data into the queue. The task will also be moved automatically from
  the Blocked state to the Ready state if the specified block time expires before data becomes available.

  Queues can have multiple readers, so it is possible for a single queue to have more than one task blocked on it
  waiting for data. When this is the case, only one task will be unblocked when data becomes available. The task that
  is unblocked will always be the highest priority task that is waiting for data. If the blocked tasks have equal
  priority, then the task that has been waiting for data the longest will be unblocked.

  Blocking on Queue Writes:
  Just as when reading from a queue, a task can optionally specify a block time when writing to a queue. In this case,
  the block time is the maximum time the task should be held in the Blocked state to wait for space to become available
  on the queue, should the queue already be full.

  Queues can have multiple writers, so it is possible for a full queue to have more than one task blocked on it
  waiting to complete a send operation. When this is the case, only one task will be unblocked when space on the queue
  becomes available. The task that is unblocked will always be the highest priority task that is waiting for space. If
  the blocked tasks have equal priority, then the task that has been waiting for space the longest will be unblocked.

  Blocking on Multiple Queues:
  Queues can be grouped into sets, allowing a task to enter the Blocked state to wait for data to become available on any
  of the queues in the set.
 ********************************************** xQueueCreate API ******************************************************
  QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength,
                            UBaseType_t uxItemSize);
  Parameters:
  uxQueueLength - The maximum number of items the queue can hold at any one time.
  uxItemSize    - The size, in bytes, required to hold each item in the queue.
                 Items are queued by copy, not by reference, so this is the number of bytes that will be copied for
                 each queued item. Each item in the queue must be the same size.
  Returns:
  If the queue is created successfully then a handle to the created queue is returned. If the memory required to
  create the queue could not be allocated then NULL is returned.

  A queue must be explicitly created before it can be used.

  Queues are referenced by handles, which are variables of type QueueHandle_t. The xQueueCreate() API function creates
  a queue and returns a QueueHandle_t that references the queue it created.

  FreeRTOS allocates RAM from the FreeRTOS heap when a queue is created. The RAM is used to hold both the queue data
  structures and the items that are contained in the queue. xQueueCreate() will return NULL if there is insufficient
  heap RAM available for the queue to be created.


  NOTE: configSUPPORT_DYNAMIC_ALLOCATION must be set to 1 in FreeRTOSConfig.h, or left undefined
       (in which case it will default to 1), for this RTOS API function to be available. The Arduino
       must include the queue.h RTOS library.
  VISIT https://www.freertos.org/a00116.html
************************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "queue.h"

static void vTaskSender(void *pvParameters);
static void vTaskReceiver(void *pvParameters);

QueueHandle_t xQueue;

void setup() {
  Serial.begin(9600);

  xQueue = xQueueCreate(5, sizeof( int ));

  if (xQueue != NULL) {
    xTaskCreate(vTaskSender, "Task Sender", 100, NULL, 1, NULL);
    xTaskCreate(vTaskReceiver, "Task Receiver", 100, NULL, 2, NULL);
  }

  // Start the scheduler to start the tasks executing:
  vTaskStartScheduler();

  // This should never reach void loop():
  while (1);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

void vTaskSender(void * pvParameters) {
  int32_t sendValue = 0;
  BaseType_t xStatus;
  for (;;) {
    sendValue = (int32_t)random(0, 1024);

    xStatus = xQueueSend(xQueue, &sendValue, (TickType_t) 0);

    Serial.print("Sent Value = ");
    Serial.println(sendValue);

    if (xStatus != pdPASS) {
      Serial.println("Could not send to the queue.\r\n");
    }
  }
}

void vTaskReceiver(void *pvParameters) {
  int receivedValue = 0;
  BaseType_t xStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);
  for (;;) {
    if (uxQueueMessagesWaiting(xQueue) != 0) {
      Serial.println("Queue should have been empty!\r\n");
    }

    xStatus = xQueueReceive(xQueue, &receivedValue, xTicksToWait);

    if (xStatus == pdPASS) {
      Serial.print("Received = ");
      Serial.print(receivedValue);
      Serial.println();
    }
    else {
      Serial.println("Could not receive from the queue.\r\n");
    }
  }
}
