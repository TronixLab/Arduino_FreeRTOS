#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define  LED  13

SemaphoreHandle_t xBinarySemaphore;

void setup() {
  Serial.begin(9600);
  pinMode(LED , OUTPUT);
  
  xBinarySemaphore = xSemaphoreCreateBinary();
  
  xTaskCreate(LedOnTask, "LedON", 100, NULL, 1, NULL);
  xTaskCreate(LedoffTask, "LedOFF", 100, NULL, 1, NULL);
  xSemaphoreGive(xBinarySemaphore);

  // Start the scheduler to start the tasks executing:
  vTaskStartScheduler();

  // This should never reach void loop():
  for ( ;; );
  return 0;
}

void loop() {}

void LedOnTask(void *pvParameters) {
  while (1) {
    xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
    Serial.println("Inside LedOnTask");
    digitalWrite(LED, HIGH);
    xSemaphoreGive(xBinarySemaphore);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void LedoffTask(void *pvParameters) {
  while (1) {
    xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
    Serial.println("Inside LedOffTask");
    digitalWrite(LED, LOW);
    xSemaphoreGive(xBinarySemaphore);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
