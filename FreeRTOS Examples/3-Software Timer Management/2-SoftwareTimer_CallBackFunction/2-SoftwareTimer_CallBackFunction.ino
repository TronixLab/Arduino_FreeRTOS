/**************************************************** The Timer ID **********************************************************
  There is an ID for each software timer, which is a tag value that can be used for any reason by the program developer.
  The ID is stored in a void pointer (void *) so that it can store an integer value directly, point to some other object,
  or be used as a function pointer.

  When the software timer is generated, an initial value is assigned to the ID, after which the ID can be modified using
  the vTimerSetTimerID() API function and compiled by the pvTimerGetTimerID() API function.
 ************************************************ vTimerSetTimerID API *******************************************************
  void vTimerSetTimerID(TimerHandle_t xTimer, void *pvNewID);
  Parameters:
  xTimer  - The timer being updated.
  pvNewID - The handle to which the timer’s identifier will be set.

  When the timer is created, a software timer is assigned an identifier (ID) and can be modified at any time using the
  vTimerSetTimerID() API feature.

  If several timers are allocated to the same callback function, the timer identifier may be examined inside the
  callback function to decide which timer has actually expired.
  VISIT https://www.freertos.org/FreeRTOS-timers-vTimerSetTimerID.html
 ************************************************ pvTimerGetTimerID API ***************************************************
  void *pvTimerGetTimerID(TimerHandle_t xTimer);
  Parameters:
  xTimer - The timer being queried.
  Returns:
  The ID assigned to the timer being queried.

  The same callback function can be assigned to more than one software timer. When that is done, the callback function \
  parameter is used to determine which software timer expired.

  In this example used two separate callback functions; one callback function was used by the one-shot timer, and the other callback
  function was used by the auto-reload timer.
  VISIT https://www.freertos.org/FreeRTOS-timers-pvTimerGetTimerID.html
****************************************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define mainONE_SHOT_TIMER_PERIOD    (pdMS_TO_TICKS(3333UL))
#define mainAUTO_RELOAD_TIMER_PERIOD (pdMS_TO_TICKS(500UL))

static void prvTimerCallback(TimerHandle_t xTimer);

static TimerHandle_t xAutoReloadTimer, xOneShotTimer;

void setup() {
  Serial.begin(9600);

  BaseType_t xTimer1Started, xTimer2Started;

  xOneShotTimer = xTimerCreate("OneShot Timer",                  // Text name for the software timer - not used by FreeRTOS.
                               mainONE_SHOT_TIMER_PERIOD,        // The software timer's period in ticks.
                               pdFALSE,                          // Setting uxAutoRealod to pdFALSE creates a one-shot software timer.
                               0,                                // This example does not use the timer id.
                               prvTimerCallback);                // The callback function to be used by the software timer being created.

  xAutoReloadTimer = xTimerCreate("AutoReload Timer",            // Text name for the software timer - not used by FreeRTOS.
                                  mainAUTO_RELOAD_TIMER_PERIOD,  // The software timer's period in ticks.
                                  pdTRUE,                        // Set uxAutoRealod to pdTRUE to create an auto-reload software timer.
                                  0,                             // This example does not use the timer id.
                                  prvTimerCallback);             // The callback function to be used by the software timer being created.

  if ((xOneShotTimer != NULL) && (xAutoReloadTimer != NULL)) {
    xTimer1Started = xTimerStart(xOneShotTimer, 0);
    xTimer2Started = xTimerStart(xAutoReloadTimer, 0);

    if ((xTimer1Started == pdPASS) && (xTimer2Started == pdPASS)) {
      // Start the scheduler to start the tasks executing:
      vTaskStartScheduler();
    }
  }
  // This should never reach void loop():
  for ( ;; );
  return 0;
}

void loop() {}

static void prvTimerCallback(TimerHandle_t xTimer) {
  TickType_t xTimeNow;
  uint32_t ulExecutionCount;
  ulExecutionCount = (uint32_t) pvTimerGetTimerID(xTimer);
  ulExecutionCount++;
  vTimerSetTimerID(xTimer, (void *) ulExecutionCount);

  xTimeNow = xTaskGetTickCount();

  if (xTimer == xOneShotTimer) {
    Serial.print("One-shot timer callback executing ");
    Serial.print(xTimeNow);
    Serial.println();
  }
  else {
    Serial.print("Auto-reload timer callback executing ");
    Serial.print(xTimeNow);
    Serial.println();
    if (ulExecutionCount == 5) {
      // Stop the auto-reload timer after it has executed 5 times.
      xTimerStop(xTimer, 0);
    }
  }
}
