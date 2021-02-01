#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "queue.h"

static void vTaskSender(void *pvParameters);
static void vTaskReceiver(void *pvParameters);

QueueHandle_t xQueue;

const int *pcTaskSender1ID = 0;
const int *pcTaskSender2ID = 1;
const int *pcTaskSender3ID = 2;

struct senderData_t {
  int senderID;
  int ucValue;
};

void setup() {
  Serial.begin(9600);

  xQueue = xQueueCreate(5, sizeof(struct senderData_t));

  if (xQueue != NULL) {
    xTaskCreate(vTaskSender, "Task Sender1", 100, (void *) pcTaskSender1ID, 2, NULL );
    xTaskCreate(vTaskSender, "Task Sender2", 100, (void *) pcTaskSender2ID, 2, NULL );
    xTaskCreate(vTaskSender, "Task Sender2", 100, (void *) pcTaskSender3ID, 2, NULL );
    xTaskCreate(vTaskReceiver, "Task Receiver", 100, NULL, 1, NULL );
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

static void vTaskSender(void *pvParameters) {
  BaseType_t xStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);
  for ( ;; )
  {
    struct senderData_t xQueueData;
    xQueueData.senderID = (void *)pvParameters;
    xQueueData.ucValue = analogRead((void *)pvParameters);

    xStatus = xQueueSend(xQueue, &xQueueData, portMAX_DELAY);

    if ( xStatus != pdPASS )
    {
      Serial.print("Could not send to the queue.\r\n");
    }
  }
}

void vTaskReceiver(void *pvParameters) {
  senderData_t xReceivedStructure;
  BaseType_t xStatus;
  for (;;)
  {
    if (uxQueueMessagesWaiting(xQueue) != 5) {
      Serial.print("Queue should have been full!\r\n");
    }

    xStatus = xQueueReceive(xQueue, &xReceivedStructure, portMAX_DELAY);

    if (xStatus == pdPASS) {
      if (xReceivedStructure.senderID == 0) {
        Serial.print("From AnalogPin A0 = ");
        Serial.print(xReceivedStructure.ucValue);
        Serial.println();
      }
      if (xReceivedStructure.senderID == 1) {
        Serial.print("From AnalogPin A1 = ");
        Serial.print(xReceivedStructure.ucValue);
        Serial.println();
      }
      if (xReceivedStructure.senderID == 2) {
        Serial.print("From AnalogPin A2 = ");
        Serial.print(xReceivedStructure.ucValue);
        Serial.println();
      }
    }
    else {
      Serial.print("Could not receive from the queue.\r\n");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
