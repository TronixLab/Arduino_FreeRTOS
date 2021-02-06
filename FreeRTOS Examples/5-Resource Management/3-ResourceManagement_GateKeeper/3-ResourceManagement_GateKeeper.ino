/***************************************** Gatekeeper Tasks ***********************************************************
 Gatekeeper tasks provide a clean method of implementing mutual exclusion without the risk of priority inversion or 
 deadlock. A gatekeeper task is a task that has sole ownership of a resource. Only the gatekeeper task is allowed to 
 access the resource directly—any other task needing to access the resource can do so only indirectly by using the 
 services of the gatekeeper.
 **********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void SenderTask(void *pvParameters);
void GateKeeperTask(void *pvParameters);

static const char *pcStringToPrint[] =
{
  "Task 1 ############################## Task1 \r\n",
  "Task 2 ------------------------------ Task2 \r\n",
};

QueueHandle_t xPrintQueue;

void setup() {
  Serial.begin(9600);
  xPrintQueue = xQueueCreate(5, sizeof(char *));
  xTaskCreate(SenderTask, "Printer Task1", 100, (void *)0, 1, NULL );
  xTaskCreate(SenderTask, "Printer Task2", 100, (void *)1, 2, NULL );
  xTaskCreate(GateKeeperTask, "GateKeeper", 100, NULL, 0, NULL);

  vTaskStartScheduler();
  for (;;);
  return 0;
}

void SenderTask(void *pvParameters) {
  int indexToString ;
  indexToString = (int)pvParameters;

  for (;;)
  {
    xQueueSend(xPrintQueue, &(pcStringToPrint[indexToString]), portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void GateKeeperTask(void *pvParameters) {
  char *pcMessageToPrint;
  for (;;)
  {
    xQueueReceive(xPrintQueue, &pcMessageToPrint, portMAX_DELAY);
    Serial.println(pcMessageToPrint);
  }
}

void loop() {}
