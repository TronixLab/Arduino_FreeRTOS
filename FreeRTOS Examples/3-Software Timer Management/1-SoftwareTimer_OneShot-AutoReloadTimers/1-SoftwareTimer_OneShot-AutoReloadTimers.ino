/************************************** Introduction to Software Timers *************************************************
 Software timers are used to schedule the execution of a function at a set time in the future, or regularly at a fixed 
 frequency. The operation performed by the software timer is called the callback function of the software timer.

 Software timers are implemented by and under the supervision of the FreeRTOS kernel. They do not need hardware support, 
 nor do they relate to hardware timers or hardware counters.

 NOTE: With the FreeRTOS principle of using an advanced design to ensure full performance, software timers do not 
       use any computing time unless a software timer callback feature is actually running.

       Software timer functionality is optional. To include software timer functionality:
       1. Build the FreeRTOS source file FreeRTOS/Source/timers.c as part of your project.
       2. Set configUSE_TIMERS to 1 in FreeRTOSConfig.h.

       Timer callback functions execute in the context of the timer service task. It is therefore essential that timer 
       callback functions never attempt to block. For example, a timer callback function must not call vTaskDelay(), 
       vTaskDelayUntil(), or specify a non zero block time when accessing a queue or a semaphore.
 VISIT https://www.freertos.org/RTOS-software-timer.html

 One-shot and Auto-reload Timers
 There are two types of software timer:
      1. One-shot timers
         Once started, a one-shot timer will execute its callback function once only. A one-shot
         timer can be restarted manually, but will not restart itself.
      2. Auto-reload timers
         Once started, an auto-reload timer will re-start itself each time it expires, resulting in
         periodic execution of its callback function.
 *************************************************** xTimerCreate API *****************************************************       
  TimerHandle_t xTimerCreate
                 (const char * const pcTimerName,
                  const TickType_t xTimerPeriod,
                  const UBaseType_t uxAutoReload,
                  void * const pvTimerID,
                  TimerCallbackFunction_t pxCallbackFunction);
  Parameters:
  pcTimerName        - A human readable text name that is assigned to the timer. This is done purely to assist debugging.
  xTimerPeriod       - The period of the timer. The period is specified in ticks, and the macro pdMS_TO_TICKS() can be used 
                       to convert a time specified in milliseconds to a time specified in ticks.
  uxAutoReload       - Set uxAutoReload to pdTRUE to create an auto-reload timer. Set uxAutoReload to pdFALSE to 
                       create a one-shot timer.
  pvTimerID          - An identifier that is assigned to the timer being created.
  pxCallbackFunction - The function to call when the timer expires.
  
  Returns:
  If NULL is returned, then the software timer cannot be created because there is insufficient heap memory available 
  for FreeRTOS to allocate the necessary data structure.

  A non-NULL value being returned indicates that the software timer has been created successfully. The returned value 
  is the handle of the created timer.

  A software timer must be explicitly created before it can be used. Software timers are referenced by variables of 
  type TimerHandle_t. xTimerCreate() is used to create a software timer and returns a TimerHandle_t to reference the 
  software timer it creates. Software timers can be created before the scheduler is running, or from a task after the 
  scheduler has been started.
*************************************************** xTimerStart API *****************************************************
  BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait);
  Parameters:
  xTimer     - The handle of the timer being started or restarted.
  xBlockTime - Specifies the time, in ticks, that the calling task should be held in the Blocked state to wait for 
               the start command to be successfully sent to the timer command queue, should the queue already be full 
               when xTimerStart() was called. xBlockTime is ignored if xTimerStart() is called before the RTOS 
               scheduler is started.
  Returns:
  pdPASS will be returned only if the ‘start a timer’ command was successfully sent to the timer command queue.

  pdFALSE will be returned if the ‘start a timer’ command could not be written to the timer command queue because the 
  queue was already full.
  VISIT https://www.freertos.org/FreeRTOS-timers-xTimerStart.html
*************************************************** xTimerCreate API *****************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define mainONE_SHOT_TIMER_PERIOD     (pdMS_TO_TICKS(3333UL))
#define mainAUTO_RELOAD_TIMER_PERIOD  (pdMS_TO_TICKS(1000UL))

static void prvOneShotTimerCallback(TimerHandle_t xTimer);
static void prvAutoReloadTimerCallback(TimerHandle_t xTimer);

void setup() {
  Serial.begin(9600);

  TimerHandle_t xAutoReloadTimer, xOneShotTimer;
  BaseType_t xTimer1Started, xTimer2Started;

  xOneShotTimer = xTimerCreate("OneShot Timer",                  // Text name for the software timer - not used by FreeRTOS.
                               mainONE_SHOT_TIMER_PERIOD,        // The software timer's period in ticks.
                               pdFALSE,                          // Setting uxAutoRealod to pdFALSE creates a one-shot software timer.
                               0,                                // This example does not use the timer id.
                               prvOneShotTimerCallback );        // The callback function to be used by the software timer being created.

  xAutoReloadTimer = xTimerCreate("AutoReload Timer",            // Text name for the software timer - not used by FreeRTOS.
                                  mainAUTO_RELOAD_TIMER_PERIOD,  // The software timer's period in ticks.
                                  pdTRUE,                        // Set uxAutoRealod to pdTRUE to create an auto-reload software timer.
                                  0,                             // This example does not use the timer id.
                                  prvAutoReloadTimerCallback );  // The callback function to be used by the software timer being created.

  if ((xOneShotTimer != NULL) && (xAutoReloadTimer != NULL)) {
    xTimer1Started = xTimerStart(xOneShotTimer, 0);
    xTimer2Started = xTimerStart(xAutoReloadTimer, 0);

    if ((xTimer1Started == pdPASS) && (xTimer2Started == pdPASS)) {
      // Start the scheduler to start the tasks executing:
      vTaskStartScheduler();
    }
  }
  // This should never reach void loop():
  while (1);
  return 0;
}

void loop() {}

static void prvOneShotTimerCallback(TimerHandle_t xTimer) {
  static TickType_t xTimeNow;

  xTimeNow = xTaskGetTickCount();

  Serial.print("One-shot timer callback executing ");
  Serial.println(xTimeNow);
}

static void prvAutoReloadTimerCallback(TimerHandle_t xTimer) {
  static TickType_t xTimeNow;

  xTimeNow = xTaskGetTickCount();

  Serial.print("Auto-reload timer callback executing ");
  Serial.println(xTimeNow);
}
