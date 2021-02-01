#include "Arduino_FreeRTOS.h"
#include"semphr.h"

static const char *pcStringToPrint[] =
{
  "Task 1 ############################## Task1 \r\n",
  "Task 2 ------------------------------ Task2 \r\n",
};

QueueHandle_t xPrintQueue;
void setup()
{
  Serial.begin(9600);
  xPrintQueue = xQueueCreate(5, sizeof(char *));
  xTaskCreate(SenderTask, "Printer Task1", 100, (void *)0, 1, NULL );
  xTaskCreate(SenderTask, "Printer Task2", 100, (void *)1, 2, NULL );
  xTaskCreate(GateKeeperTask, "GateKeeper", 100, NULL, 0, NULL);
}

void SenderTask(void *pvParameters)
{
  int indexToString ;
  indexToString = (int)pvParameters;

  while (1)
  {
    xQueueSend(xPrintQueue, &(pcStringToPrint[indexToString]), portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void GateKeeperTask(void *pvParameters)
{
  char *pcMessageToPrint;
  while (1)
  {
    xQueueReceive(xPrintQueue, &pcMessageToPrint, portMAX_DELAY);
    Serial.println(pcMessageToPrint);
  }
}

void loop() {}
