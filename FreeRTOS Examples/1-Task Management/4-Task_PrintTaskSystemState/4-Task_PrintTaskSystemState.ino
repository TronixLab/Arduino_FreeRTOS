/****************************************** vTaskList API ************************************************************
   void vTaskList(char *pcWriteBuffer);
   Parameters:
   pcWriteBuffer - A buffer into which the above mentioned details will be written, in ASCII form. This buffer is
                   assumed to be large enough to contain the generated report. Approximately 40 bytes per task
                   should be sufficient.

   VTaskList() calls uxTaskGetSystemState(), then formats the raw data generated by uxTaskGetSystemState() into a
   human readable (ASCII) table that shows the status of each task, including the high water stack task.
   The following letters are used in the ASCII table to show the status of the task:
   ‘B’ – Blocked
   ‘R’ – Ready
   ‘D’ – Deleted (waiting clean up)
   ‘S’ – Suspended, or Blocked without a timeout

   NOTE: configUSE_TRACE_FACILITY and configUSE_STATS_FORMATTING_FUNCTIONS must be defined as 1 in FreeRTOSConfig.h
         for this function to be available. This function will override the duration of the interrupts. It is not
         intended for regular runtime usage of applications, but as a debugging support.
   VISIT https://www.freertos.org/a00021.html#vTaskList
*********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);

TaskHandle_t xTask1Handle;

const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";
const char *pcTextForTask3 = "Task 3 is running\r\n";

char ptrTaskList[250];

void setup() {
  Serial.begin(9600);

  // Attach interrupt to print Task status:
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), xPrintTaskList, RISING);

  // Create Tasks:
  xTaskCreate(vTask1, "Task1", 100, (void*)pcTextForTask1, 1, &xTask1Handle);
  xTaskCreate(vTask2, "Task2", 100, (void*)pcTextForTask2, 2, NULL);
  xTaskCreate(vTask3, "Task3", 100, (void*)pcTextForTask3, 3, NULL);

  // Start the scheduler to start the tasks executing:
  vTaskStartScheduler();

  // This should never reach void loop():
  while (1);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

static void xPrintTaskList() {
  vTaskList(ptrTaskList);
  Serial.println(F("*******************************************************************"));
  Serial.println(F("Task                                   State   Prio    Stack    Num"));
  Serial.println(F("*******************************************************************"));
  Serial.print(ptrTaskList);
  Serial.println(F("*******************************************************************"));
}

static void vTask1(void* pvParameters) {
  char *pcTask1Name;
  pcTask1Name = (char *) pvParameters;
  for (;;)
  {
    Serial.println(pcTask1Name);
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}

static void vTask2(void* pvParameters) {
  char *pcTask2Name;
  pcTask2Name = (char *) pvParameters;
  for (;;)
  {
    Serial.println(pcTask2Name);
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

static void vTask3(void* pvParameters) {
  char *pcTask3Name;
  pcTask3Name = (char *) pvParameters;
  char suspendFlag = 0;
  for (;;)
  {
    Serial.println(pcTask3Name);

    if (suspendFlag == 1) {
      vTaskSuspend(xTask1Handle);
      suspendFlag = 0;
    }
    else {
      vTaskResume(xTask1Handle);
      suspendFlag = 1;
    }
    vTaskDelay(400 / portTICK_PERIOD_MS);
  }
}