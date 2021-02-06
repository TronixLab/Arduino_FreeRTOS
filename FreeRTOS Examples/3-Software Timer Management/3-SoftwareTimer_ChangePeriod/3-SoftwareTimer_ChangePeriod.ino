/**************************************************** Changing the Period of a Timer *******************************************************
 Every official FreeRTOS port is provided with one or more example projects. Most example projects are self-checking, 
 and an LED is used to give visual feedback of the project’s status; if the self-checks have always passed then the 
 LED is toggled slowly, if a self-check has ever failed then the LED is toggled quickly.

 Some example projects perform the self-checks in a task, and use the vTaskDelay() function to control the rate at 
 which the LED toggles. Other example projects perform the self-checks in a software timer callback function, and use 
 the timer’s period to control the rate at which the LED toggles.
 **************************************************** xTimerChangePeriod API ********************************************
 BaseType_t xTimerChangePeriod(TimerHandle_t xTimer,
                               TickType_t xNewPeriod,
                               TickType_t xBlockTime);
 Parameters:
 xTimer              - The handle of the software timer being updated with a new period value. The handle will have 
                       been returned from the call to xTimerCreate() used to create the software timer.
 xTimerPeriodInTicks - The new period for the software timer, specified in ticks. The pdMS_TO_TICKS() macro can be 
                       used to convert a time specified in milliseconds into a time specified in ticks.
 xBlockTime          - Specifies the time, in ticks, that the calling task should be held in the Blocked state to wait 
                       for the change period command to be successfully sent to the timer command queue, should the 
                       queue already be full when xTimerChangePeriod() was called.
 */
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS(500UL)
#define mainAUTO_RELOAD_TIMER_PERIOD2 pdMS_TO_TICKS(1000UL)

TimerHandle_t xAutoReloadTimer;
BaseType_t xTimer2Started;

void setup()
{
  Serial.begin(9600);
  xAutoReloadTimer = xTimerCreate("AutoReload", mainAUTO_RELOAD_TIMER_PERIOD, pdTRUE, 0, prvTimerCallback);

  if ((xAutoReloadTimer != NULL)) {
    xTimer2Started = xTimerStart( xAutoReloadTimer, 0);
    if ((xTimer2Started == pdPASS)) {
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

  Serial.print("Auto-reload timer callback executing ");
  Serial.println(xTimeNow / 31);
  
  if (ulExecutionCount >= 5) {
    xTimerChangePeriod(xAutoReloadTimer, 
                        mainAUTO_RELOAD_TIMER_PERIOD2, 
                        0 ); 
  }
}
