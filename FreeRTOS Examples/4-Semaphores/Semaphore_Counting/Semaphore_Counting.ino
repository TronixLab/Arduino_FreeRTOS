#include "Arduino_FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t xCountingSemaphore;

void setup() 
{
  Serial.begin(9600); // Enable serial communication library.
  pinMode(LED_BUILTIN, OUTPUT);

 // Create task for Arduino led 
  xTaskCreate(Task1, // Task function
              "Ledon", // Task name
              128, // Stack size 
              NULL, 
              0 ,// Priority
              NULL );
   xTaskCreate(Task2, // Task function
              "Ledoff", // Task name
              128, // Stack size 
              NULL, 
              0, // Priority
              NULL );
   xCountingSemaphore = xSemaphoreCreateCounting(1,1);
   xSemaphoreGive(xCountingSemaphore);

}

void loop() {}



void Task1(void *pvParameters)
{
  (void) pvParameters;

  for (;;) 
    {
      xSemaphoreTake(xCountingSemaphore, portMAX_DELAY); 
      Serial.println("Inside Task1 and Serial monitor Resource Taken");
      digitalWrite(LED_BUILTIN, HIGH);
      xSemaphoreGive(xCountingSemaphore);
      vTaskDelay(1);
    }
  }

void Task2(void *pvParameters)
{
  (void) pvParameters;
  for (;;) 
    {
      xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);
      Serial.println("Inside Task2 and Serial monitor Resource Taken");
      digitalWrite(LED_BUILTIN, LOW);
      xSemaphoreGive(xCountingSemaphore);
      vTaskDelay(1);

  }
}
