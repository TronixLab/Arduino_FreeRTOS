/*** Deadlock (or Deadly Embrace) ******
  ‘Deadlock’ is another potential pitfall of using mutexes for mutual exclusion. Deadlock is sometimes also known by
  the more dramatic name ‘deadly embrace’.

  Deadlock occurs when two tasks cannot proceed because they are both waiting for a resource that is held by the other. 
  Consider the following scenario where Task A and Task B both need to acquire mutex X and mutex Y in order to perform 
  an action:
  1. Task A executes and successfully takes mutex X.
  2. Task A is pre-empted by Task B.
  3. Task B successfully takes mutex Y before attempting to also take mutex X—but mutex X is held by Task A so is not 
     available to Task B. Task B opts to enter the Blocked state to wait for mutex X to be released.
  4. Task A continues executing. It attempts to take mutex Y—but mutex Y is held by Task B, so is not available to 
     Task A. Task A opts to enter the Blocked state to wait for mutex Y to be released.

  At the end of this scenario, Task A is waiting for a mutex held by Task B, and Task B is waiting for a mutex held by 
  Task A. Deadlock has occurred because neither task can proceed.
*/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "semphr.h"

SemaphoreHandle_t xMutex;
SemaphoreHandle_t yMutex;

void setup() {
  Serial.begin(9600);
  xMutex = xSemaphoreCreateMutex();
  yMutex = xSemaphoreCreateMutex();

  xTaskCreate(Task1, "Task1", 100, NULL, 1, NULL);

  xSemaphoreGive(xMutex);
  xSemaphoreGive(yMutex);

  // Start the scheduler so the created tasks start executing.
  vTaskStartScheduler();

  // The line should never be reached void loop().
  for (;;);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}
void Task1(void *pvParameters) {
  for ( ;; )
  {
    xSemaphoreTake(xMutex, portMAX_DELAY);
    Serial.println("Inside Task1");
    xTaskCreate(Task2, "Task2", 100, NULL, 2, NULL);
    Serial.println("Task1 attempting to take yMutex");
    xSemaphoreTake(yMutex, portMAX_DELAY);
    xSemaphoreGive(xMutex);
    vTaskDelay(1);
  }
}

void Task2(void *pvParameters) {
  for ( ;; )
  {
    xSemaphoreTake(yMutex, portMAX_DELAY);
    Serial.println("Inside Task2");
    Serial.println("Task2 attempting to take xMutex");
    xSemaphoreTake(xMutex, portMAX_DELAY);
    xSemaphoreGive(yMutex);
    vTaskDelay(1);
  }
}
