/************************************** Receiving Data From Multiple Sources ***************************************************
 In FreeRTOS designs, it is commonplace that a task receives data from more than one source. To decide how the data 
 should be handled, the receiving task needs to know where the data came from. An simple architecture option is to 
 move structures with both the value of the data and the source of the data found in the fields of the structure 
 using a single queue.
************************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "queue.h"

static void vTaskSender1(void *pvParameters);
static void vTaskSender2(void *pvParameters);
static void vTaskReceiver(void *pvParameters);

QueueHandle_t xQueue;

const int *pcTaskSender1ID = 1;
const int *pcTaskSender2ID = 2;

struct senderData_t {
  int senderID;
  int ucValue;
};

void setup() {
  Serial.begin(9600);

  xQueue = xQueueCreate(5, sizeof(struct senderData_t));

  if (xQueue != NULL) {
    xTaskCreate(vTaskSender1, "Task1 Sender", 100, (void *) pcTaskSender1ID, 2, NULL);
    xTaskCreate(vTaskSender2, "Task2 Sender", 100, (void *) pcTaskSender2ID, 2, NULL);
    xTaskCreate(vTaskReceiver, "Task Receiver", 100, NULL, 1, NULL);
  }

  // Start the scheduler to start the tasks executing:
  vTaskStartScheduler();

  // This should never reach void loop():
  while (1);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

void vTaskSender1(void * pvParameters) {
  BaseType_t xStatus;
  for (;;) {
    struct senderData_t xQueueData;
    xQueueData.senderID = (void *)pvParameters;
    xQueueData.ucValue = 100;

    xStatus = xQueueSend(xQueue, &xQueueData, portMAX_DELAY);
    taskYIELD();

    Serial.print("Task Sender ");
    Serial.print(xQueueData.senderID);
    Serial.print(" data = ");
    Serial.println(xQueueData.ucValue);

    if (xStatus != pdPASS) {
      Serial.println("Could not send to the queue.\r\n");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void vTaskSender2(void * pvParameters) {
  BaseType_t xStatus;
  for (;;) {
    struct senderData_t xQueueData;
    xQueueData.senderID = (void *)pvParameters;
    xQueueData.ucValue = 200;

    xStatus = xQueueSend(xQueue, &xQueueData, portMAX_DELAY);

    Serial.print("Task Sender ");
    Serial.print(xQueueData.senderID);
    Serial.print(" data = ");
    Serial.println(xQueueData.ucValue);

    if (xStatus != pdPASS) {
      Serial.println("Could not send to the queue.\r\n");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

static void vTaskReceiver(void *pvParameters) {
  BaseType_t xStatus;
  for (;;)
  {
    struct senderData_t xReceivedStructure;
    if (uxQueueMessagesWaiting(xQueue) != 5) {
      Serial.print("Queue should have been full!\r\n");
    }

    xStatus = xQueueReceive(xQueue, &xReceivedStructure, portMAX_DELAY);
    if (xStatus == pdPASS) {
      if (xReceivedStructure.senderID == 1) {
        Serial.print("From Task Sender 1 = ");
        Serial.print(xReceivedStructure.ucValue);
        Serial.println();
      }
      if (xReceivedStructure.senderID == 2) {
        Serial.print("From Task Sender 2 = ");
        Serial.print(xReceivedStructure.ucValue);
        Serial.println();
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
