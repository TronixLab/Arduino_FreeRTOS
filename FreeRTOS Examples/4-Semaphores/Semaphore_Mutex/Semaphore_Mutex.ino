//We will start by adding header files of FreeRTOS and semaphore
#include "Arduino_FreeRTOS.h"
#include "semphr.h"
//create handle for the mutex. It will be used to reference mutex
SemaphoreHandle_t  xMutex;

void setup()
{
  // Enable serial module of Arduino with 9600 baud rate
  Serial.begin(9600);
  // create mutex and assign it a already create handler
  xMutex = xSemaphoreCreateMutex();
  // create two instances of task "OutputTask" which are used to display string on
  // arduino serial monitor. We passed strings as a paramter to these tasks such as ""Task 1 //#####################Task1" and "Task 2 ---------------------Task2". Priority of one //instance is higher than the other
  xTaskCreate(OutputTask, "Printer Task 1", 100, "Task 1 #####################Task1 \r\n", 1, NULL);
  xTaskCreate(OutputTask, "Printer Task 2", 100, "Task 2 ---------------------Task2 \r\n", 2, NULL);
}

// this is a definition of tasks
void OutputTask(void *pvParameters)
{
  char *pcStringToPrint;
  pcStringToPrint = (char *)pvParameters;
  while (1)
  {
    printer(pcStringToPrint);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
// this printer task send data to arduino serial monitor
//aslo it is shared resource between both instances of the tasks
void printer(const char* pcString)
{
  // take mutex
  xSemaphoreTake(xMutex, portMAX_DELAY);
  Serial.println(pcString); // send string to serial monitor
  xSemaphoreGive(xMutex); // release mutex
}
void loop() {}
