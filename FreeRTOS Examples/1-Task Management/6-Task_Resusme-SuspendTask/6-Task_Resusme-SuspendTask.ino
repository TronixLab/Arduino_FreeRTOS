/********************************************** Suspend and Resume Task *******************************************************
 ************************************************* vTaskSuspend API ***************************************************
  void vTaskSuspend(TaskHandle_t xTaskToSuspend);
  Parameters:
  xTaskToSuspend - Handle to the task being suspended. Passing a NULL handle will cause the calling task to be suspended.

  Suspend some kind of task. When a task is stopped, no time for microcontroller processing, no matter what its priority.
  Calls to vTaskSuspend are not accumulative, e.g. calling vTaskSuspend() twice on the same task always requires only
  one call to vTaskResume() to prepare the suspended task.
  VISIT https://www.freertos.org/a00130.html
 ************************************************** vTaskResume API ***************************************************
  void vTaskResume(TaskHandle_t xTaskToResume);
  Parameters:
  xTaskToResume - Handle to the task being readied.

  Resumes the suspended task. A task that has been suspended by one or even more calls to vTaskSuspend() will be made
  accessible again by a single call to vTaskResume ().

  NOTE: INCLUDE_vTaskSuspend must be defined as 1 for this function to be available.
  VISIT https://www.freertos.org/a00131.html
 **************************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"

void vTaskBlinkLED(void *pvParameters);
void vTaskSerialRcv(void *pvParameters);

TaskHandle_t xTaskBlinkLEDHandle;

void setup() {
  Serial.begin(9600);

  // Create Task:
  xTaskCreate(vTaskBlinkLED, "TASK BLINK LED", 100, NULL, 1, &xTaskBlinkLEDHandle);
  xTaskCreate(vTaskSerialRcv, "TASK SERIAL RCV CMD", 100, NULL, 1, NULL);

  // Start the scheduler to start the tasks executing:
  vTaskStartScheduler();

  // This should never reach void loop():
  while (1);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

void vTaskSerialRcv(void* pvParameters) {
  for (;;)
  {
    // Send "s" or "r" through the serial port to control the suspend and resume of the LED light task.
    while (Serial.available() > 0) {
      switch (Serial.read()) {
        case 's':
          vTaskSuspend(xTaskBlinkLEDHandle);
          Serial.println("Suspend LED Blink!");
          break;
        case 'r':
          vTaskResume(xTaskBlinkLEDHandle);
          Serial.println("Resume LED Blink!");
          break;
      }
      vTaskDelay(1);
    }
  }
}

void vTaskBlinkLED(void *pvParameters) {
  pinMode(LED_BUILTIN, OUTPUT);
  bool LEDState = false;
  for (;;) 
  {
    LEDState = !(LEDState);
    digitalWrite(LED_BUILTIN, LEDState);   
    vTaskDelay(250 / portTICK_PERIOD_MS); 
  }
}
