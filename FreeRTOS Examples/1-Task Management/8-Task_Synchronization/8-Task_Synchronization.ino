/**************************************** Task Synchronization Using an Event Group ****************************************************
  Often the design of an application allows two or more tasks to be coordinated. Consider, for example, a design
  where Task A receives an event, and then transfers some of the processing needed by the event to three other tasks:
  Task B, Task C, and Task D. If task A is unable to receive another event until tasks B, C, and D have completed all
  of the previous event's processing, then all four tasks must synchronize with each other. The synchronization point
  of each task will be after that task has been completed and cannot begin until each of the other tasks has done the
  same. Only after all four tasks have reached their synchronization point will Task A receive another event.
  ************************************************ xEventGroupSync API ************************************************
  EventBits_t xEventGroupSync(EventGroupHandle_t xEventGroup,
                              const EventBits_t uxBitsToSet,
                              const EventBits_t uxBitsToWaitFor,
                              TickType_t xTicksToWait);
  Parameters:
  xEventGroup     - The handle of the event group in which to set and then evaluate the event bits. The handle for the
                    event group was returned from the xEventGroupCreate() call that was used to create the event group.
  uxBitsToSet     - A bit mask that specifies the event bit, or event bits, to set to 1 in the event group.
  uxBitsToWaitFor - A bit mask that specifies the event bit, or event bits, to test in the event group.
  xTicksToWait    - The maximum amount of time the task should remain in the Blocked state to wait for its unblock
                    condition to be met.

  Returns:
  The value of the event group at the time either the bits being waited for became set, or the block time expired.
  Test the return value to know which bits were set.

  If xEventGroupSync() returned because its timeout expired then not all the bits being waited for will be set.

  If xEventGroupSync() returned because all the bits it was waiting for were set then the returned value is the event
  group value before any bits were automatically cleared.

  NOTE: To use the event groups feature of FreeRTOS, simply add event_groups.h library in your Arduino source file.
  VISIT https://www.freertos.org/xEventGroupSync.html
*********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

void vTaskSync(void *pvParameters);

#define TASK1_BIT   (1UL << 0UL) // Event bit 0
#define TASK2_BIT   (1UL << 1UL) // Event bit 1
#define TASK3_BIT   (1UL << 2UL) // Event bit 2

EventGroupHandle_t  xEventGroup;

void setup() {
  Serial.begin(9600);

  xEventGroup  =  xEventGroupCreate();

  xTaskCreate(vTaskSync, "TASK1", 100, (void*)TASK1_BIT, 1, NULL);
  xTaskCreate(vTaskSync, "TASK2", 100, (void*)TASK2_BIT, 1, NULL );
  xTaskCreate(vTaskSync, "TASK3", 100, (void*)TASK3_BIT, 1, NULL);

  // Start the scheduler to start the tasks executing:
  vTaskStartScheduler();

  // This should never reach void loop():
  while (1);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

static void vTaskSync( void *pvParameters ) {
  const TickType_t xMaxDelay = pdMS_TO_TICKS(4000UL);
  const TickType_t xMinDelay = pdMS_TO_TICKS(200UL);
  TickType_t xDelayTime;
  EventBits_t uxThisTasksSyncBit;
  const EventBits_t uxAllSyncBits = (TASK1_BIT |
                                     TASK2_BIT |
                                     TASK3_BIT);

  uxThisTasksSyncBit = (EventBits_t) pvParameters;
  for ( ;; )
  {
    xDelayTime = (rand() % xMaxDelay) + xMinDelay;
    vTaskDelay(xDelayTime);

    Serial.print(pcTaskGetName(NULL));
    Serial.print(" reached sync point.");
    Serial.println();

    xEventGroupSync(xEventGroup,          // The event group used to synchronize
                    uxThisTasksSyncBit,   // The bit set by this task to indicate it has reached the synchronization point
                    uxAllSyncBits,        // The bits to wait for, one bit for each task taking part in the synchronization
                    portMAX_DELAY);       // Wait indefinitely for all three tasks to reach the synchronization point

    Serial.print(pcTaskGetName(NULL));
    Serial.print(" exited sync point.");
    Serial.println();
  }
}
