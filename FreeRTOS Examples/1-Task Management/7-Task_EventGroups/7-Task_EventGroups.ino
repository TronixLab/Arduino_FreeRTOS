/*************************************************** Event Groups ****************************************************
  Event groups are another feature of FreeRTOS that enables events to be communicated to tasks. Requires a task to
  wait in the Blocked State for a combination of one or more events to occur. Event groups release all activities that
  were waiting for the same event, or a mix of events, when the event occurred.

  Event Groups, Event Flags and Event Bits:
  An event ‘flag’ is a Boolean (1 or 0) value used to indicate if an event has occurred or not.
  An event ‘group’ is a set of event flags.

  An event flag can only be 1 or 0, allowing the state of an event flag to be stored in a single bit, and the state
  of all the event flags in an event group to be stored in a single variable; the state of each event flag in an
  event group is represented by a single bit in a variable of type EventBits_t.

  The number of event bits in an event group is dependent on the configUSE_16_BIT_TICKS compile time configuration
  constant within FreeRTOSConfig.h file:
   - If configUSE_16_BIT_TICKS is 1, then each event group contains 8 usable event bits.
   - If configUSE_16_BIT_TICKS is 0, then each event group contains 24 usable event bits.
 ************************************************* xEventGroupCreate API ****************************************************
  EventGroupHandle_t xEventGroupCreate(void);
  Parameters:
  None
  Returns:
  If the event group was created then a handle to the event group is returned. If there was insufficient FreeRTOS
  heap available to create the event group then NULL is returned.

  An event group must be specifically set up for it to be used. Activity classes are referenced using
  EventGroupHandle_t variables. The xEventGroupCreate() API function is used to create an event group and returns
  EventGroupHandle t to refer to the event group it creates.

  Each event group needs a minimal amount of RAM that is used to maintain the status of the event group. If an event
  group is formed using xEventGroupCreate() then the necessary RAM will be allocated automatically from the FreeRTOS
  heap. Event groups are contained in EventBits_t variables.
  VISIT https://www.freertos.org/xEventGroupCreate.html
 ********************************************** xEventGroupSetBits API ***********************************************
  EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToSet);
  Parameters:
  xEventGroup - The event group in which the bits are to be set. The event group must have previously been created
               using a call to xEventGroupCreate().
  uxBitsToSet - A bitwise value that indicates the bit or bits to set in the event group.
  Returns:
  The value of the event group at the time the call to xEventGroupSetBits() returns.

  The xEventGroupSetBits() API function sets one or more bits in an event group, and is typically used to notify a
  task that the events represented by the bit, or bits, being set has occurred.
  VISIT https://www.freertos.org/xEventGroupSetBits.html
 ********************************************** xEventGroupWaitBits API ***********************************************
  EventBits_t xEventGroupWaitBits(
                       const EventGroupHandle_t xEventGroup,
                       const EventBits_t uxBitsToWaitFor,
                       const BaseType_t xClearOnExit,
                       const BaseType_t xWaitForAllBits,
                       TickType_t xTicksToWait);
  Parameters:
  xEventGroup     - The event group in which the bits are being tested. The event group must have previously been created
                   using a call to xEventGroupCreate().
  uxBitsToWaitFor - A bitwise value that indicates the bit or bits to test inside the event group
  xClearOnExit    - If xClearOnExit is set to pdTRUE then any bits set in the value passed as the uxBitsToWaitFor
                   parameter will be cleared in the event group before xEventGroupWaitBits() returns if
                   xEventGroupWaitBits() returns for any reason other than a timeout.

                   If xClearOnExit is set to pdFALSE then the bits set in the event group are not altered when the
                   call to xEventGroupWaitBits() returns.
  xWaitForAllBits - If xWaitForAllBits is set to pdTRUE then xEventGroupWaitBits() will return when either all the
                   bits set in the value passed as the uxBitsToWaitFor parameter are set in the event group or the
                   specified block time expires.

                   If xWaitForAllBits is set to pdFALSE then xEventGroupWaitBits() will return when any of the bits
                   set in the value passed as the uxBitsToWaitFor parameter are set in the event group or the
                   specified block time expires.

  xTicksToWait    - The maximum amount of time (specified in ‘ticks’) to wait for one/all (depending on the
                   xWaitForAllBits value) of the bits specified by uxBitsToWaitFor to become set.

  Returns:
  If xEventGroupWaitBits() returned because the calling task’s unblock condition was met, then the returned value is
  the value of the event group at the time the calling task’s unblock condition was met.

  If xEventGroupWaitBits() returned because the block time specified by the xTicksToWait parameter expired, then the
  returned value is the value of the event group at the time the block time expired.

  NOTE: To use the event groups feature of FreeRTOS, simply add event_groups.h library in your Arduino source file.
  VISIT https://www.freertos.org/xEventGroupWaitBits.html
*********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);
void vTaskGroupEvent(void *pvParameters);

#define TASK1_BIT   (1UL << 0UL) // Event bit 0
#define TASK2_BIT   (1UL << 1UL) // Event bit 1
#define TASK3_BIT   (1UL << 2UL) // Event bit 2

EventGroupHandle_t  xEventGroup;

const TickType_t xDelay100ms = pdMS_TO_TICKS(100);
const TickType_t xDelay500ms = pdMS_TO_TICKS(500);
const TickType_t xDelay1000ms = pdMS_TO_TICKS(1000);

const char *pcTextForTask1 = "Task 1 is running\r";
const char *pcTextForTask2 = "Task 2 is running\r";
const char *pcTextForTask3 = "Task 3 is running\r";

void setup() {
  Serial.begin(9600);
  
  xEventGroup  =  xEventGroupCreate();

  xTaskCreate(vTask1, "TASK1", 100, (void*)pcTextForTask1, 1, NULL);
  xTaskCreate(vTask2, "TASK2", 100, (void*)pcTextForTask2, 1, NULL );
  xTaskCreate(vTask3, "TASK3", 100, (void*)pcTextForTask3, 1, NULL);
  xTaskCreate(vTaskGroupEvent, "TASK EVENT GROUP", 100, NULL, 1, NULL);

  // Start the scheduler to start the tasks executing:
  vTaskStartScheduler();

  // This should never reach void loop():
  while (1);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

void vTask1(void *pvParameters) {
  char *pcTask1Name;
  pcTask1Name = (char *) pvParameters;
  for (;;)
  {
    xEventGroupSetBits(xEventGroup, TASK1_BIT);
    Serial.println(pcTask1Name);
    vTaskDelay(xDelay100ms);
  }
}

void vTask2(void *pvParameters) {
  char *pcTask2Name;
  pcTask2Name = (char *) pvParameters;
  for (;;)
  {
    xEventGroupSetBits(xEventGroup, TASK2_BIT);
    Serial.println(pcTask2Name);
    vTaskDelay(xDelay500ms);
  }
}

void vTask3(void *pvParameters) {
  char *pcTask3Name;
  pcTask3Name = (char *) pvParameters;
  for (;;)
  {
    xEventGroupSetBits(xEventGroup, TASK3_BIT);
    Serial.println(pcTask3Name);
    vTaskDelay(xDelay1000ms);
  }
}

void vTaskGroupEvent(void *pvParameters) {
  const EventBits_t xBitsToWaitFor  = (TASK1_BIT | TASK2_BIT | TASK3_BIT);
  EventBits_t xEventGroupValue;
  for (;;)
  {
    xEventGroupValue  = xEventGroupWaitBits(xEventGroup,
                                            xBitsToWaitFor,
                                            pdTRUE,
                                            pdTRUE,
                                            portMAX_DELAY
                                           );
    if ((xEventGroupValue & TASK1_BIT) != 0) {
      Serial.println();
      Serial.println();
      Serial.print("Task1, ");
    }
    if ((xEventGroupValue & TASK2_BIT != 0)) {
      Serial.print("Task2, and ");
    }
    if ((xEventGroupValue & TASK3_BIT != 0)) {
      Serial.print("Task3 event has occured");
      Serial.println();
      Serial.println();
    }
  }
}
