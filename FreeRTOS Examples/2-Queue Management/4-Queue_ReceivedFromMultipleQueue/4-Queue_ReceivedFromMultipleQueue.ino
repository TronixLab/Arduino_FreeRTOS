/***************************************** Receiving From Multiple Queues ***************************************************
  In order to obtain data of various sizes, data of different meanings, and data from different sources, application
  designs often involve a single job. The previous section showed how using a single queue that receives structures
  can accomplish this in a neat and efficient way. However, often the designer of an application deals with
  restrictions that restrict their design choices, making it appropriate for certain data sources to use a separate
  queue. For instance, the integration of 3rd party application into a design might assume the existence of a
  dedicated queue. It is possible to use a 'queue set' in such situations.

  Queue sets allow a task to receive data from more than one queue without the task polling each queue in turn to determine
  which, if any, contains data.

  A system that uses a queue set to receive data from various sources is less neat and less effective than a design
  that achieves the same functionality by using a single queue that receives structures. For this purpose, it is
  recommended that the queue sets be used only if the design constraints make their use completely mandatory.

  The following sections describe how to use a queue set by:
  1. Creating a queue set.
  2. Adding queues to the set.
  3. Reading from the queue set to determine which queues within the set contain data.

  NOTE: Queue set functionality is enabled by setting the configUSE_QUEUE_SETS compile time configuration constant
       to 1 in FreeRTOSConfig.h.
 ********************************************* xQueueCreateSet API ****************************************************
 QueueSetHandle_t xQueueCreateSet(const UBaseType_t uxEventQueueLength);
 Parameters:
 uxEventQueueLength - Queue sets store events that occur on the queues and semaphores contained in the set. 
                      uxEventQueueLength specifies the maximum number of events that can be queued at once.
 Returns:
 If NULL is returned, then the queue set cannot be created because there is insufficient heap memory available for 
 FreeRTOS to allocate the queue set data structures and storage area.

 A non-NULL value being returned indicates that the queue set has been created successfully. The returned value 
 should be stored as the handle to the created queue set.

 A queue set must be explicitly created before it can be used. Queues sets are referenced by handles, which are 
 variables of type QueueSetHandle_t. The xQueueCreateSet() API function creates a queue set and returns a 
 QueueSetHandle_t that references the queue set it created.
 
 Queue sets provide a mechanism to allow an RTOS task to block (pend) on a read operation from multiple RTOS queues 
 or semaphores simultaneously. Note that there are simpler alternatives to using queue sets.
 VISIT https://www.freertos.org/xQueueCreateSet.html
 ********************************************* xQueueAddToSet API ****************************************************
 BaseType_t xQueueAddToSet(QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet);
 Parameters:
 xQueueOrSemaphore - The handle of the queue or semaphore being added to the queue set (cast to an QueueSetMemberHandle_t type).
 xQueueSet         - The handle of the queue set to which the queue or semaphore is being added.
 Returns:
 There are two possible return values:
 1. pdPASS will be returned only if the queue or semaphore was successfully added to the queue set.
 2. pdFAIL will be returned if the queue or semaphore could not be added to the queue set.

 Adds an RTOS queue or semaphore to a queue set that was previously created by a call to xQueueCreateSet().

 A receive (in the case of a queue) or take (in the case of a semaphore) operation must not be performed on a member 
 of a queue set unless a call to xQueueSelectFromSet() has first returned a handle to that set member.

 VISIT https://www.freertos.org/xQueueAddToSet.html
 ********************************************* xQueueSelectFromSet API ****************************************************
 QueueSetMemberHandle_t xQueueSelectFromSet(QueueSetHandle_t xQueueSet, const TickType_t xTicksToWait);
 Parameters:
 xQueueSet    - The queue set on which the task will (potentially) block.
 xTicksToWait - The maximum time, in ticks, that the calling task will remain in the Blocked state 
                (with other tasks executing) to wait for a member of the queue set to be ready for a successful 
                queue read or semaphore take operation.
 Returns:
 A return value that is not NULL will be the handle of a queue or semaphore that is known to contain data.
 If the return value is NULL then a handle could not be read from the queue set.

 xQueueSelectFromSet() reads a queue handle from the queue set. When a queue or semaphore that is a member of a set 
 receives data, the handle of the receiving queue or semaphore is sent to the queue set, and returned when a task 
 calls xQueueSelectFromSet(). If a handle is returned from a call to xQueueSelectFromSet() then the queue or 
 semaphore referenced by the handle is known to contain data and the calling task must then read from the queue or 
 semaphore directly.
 VISIT https://www.freertos.org/xQueueSelectFromSet.html
****************************************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "queue.h"

static QueueHandle_t xQueue1 = NULL;
static QueueHandle_t xQueue2 = NULL;
static QueueSetHandle_t xQueueSet = NULL;


static void vTaskSender1(void *pvParameters);
static void vTaskSender2(void *pvParameters);
static void vTaskReceiver(void *pvParameters);

void setup()
{
  Serial.begin(9600);
  xQueue1 = xQueueCreate( 1, sizeof( char * ) );
  xQueue2 = xQueueCreate( 1, sizeof( char * ) );
  xQueueSet = xQueueCreateSet( 1 * 2 );
  xQueueAddToSet( xQueue1, xQueueSet );
  xQueueAddToSet( xQueue2, xQueueSet );

  xTaskCreate(vTaskSender1, "Task Sender1", 100, NULL, 1, NULL);
  xTaskCreate(vTaskSender2, "Task Sender2", 100, NULL, 1, NULL);
  xTaskCreate(vTaskReceiver, "Task Receiver", 100, NULL, 2, NULL );

  // Start the scheduler to start the tasks executing:
  vTaskStartScheduler();

  // This should never reach void loop():
  while (1);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

void vTaskSender1(void* pvParameters) {
  const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  const char * const pcMessage = "Message from vTaskSender1\r\n";
  while (1)
  {


    xQueueSend( xQueue1, &pcMessage, 0 );
    vTaskDelay(xBlockTime);
  }
}

void vTaskSender2(void* pvParameters) {
  const TickType_t xBlockTime = pdMS_TO_TICKS( 1000 );
  const char * const pcMessage = "Message from vTaskSender2\r\n";
  while (1)
  {

    xQueueSend( xQueue2, &pcMessage, 0 );
    vTaskDelay(xBlockTime);
  }

}

void vTaskReceiver( void *pvParameters ) {
  QueueHandle_t xQueueThatContainsData;
  char *pcReceivedString;
  for ( ;; )
  {
    xQueueThatContainsData = ( QueueHandle_t )
                             xQueueSelectFromSet( xQueueSet, portMAX_DELAY );
    xQueueReceive( xQueueThatContainsData, &pcReceivedString, 0 );
    Serial.println(pcReceivedString);
  }
}
