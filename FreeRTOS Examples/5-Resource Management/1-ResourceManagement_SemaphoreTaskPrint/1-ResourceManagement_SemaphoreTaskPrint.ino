/**************************************** Mutexes (Binary Semaphores) ******************************************************
  A Mutex is a special type of binary semaphore that is used to control access to a resource that is shared between two
  or more tasks. The word MUTEX originates from ‘MUTual EXclusion’.

  When used in a mutual exclusion scenario, the mutex can be thought of as a token that is associated with the resource
  being shared. For a task to access the resource legitimately, it must first successfully ‘take’ the token (be the
  token holder). When the token holder has finished with the resource, it must ‘give’ the token back. Only when the
  token has been returned can another task successfully take the token, and then safely access the same shared resource.

  NOTE: configUSE_MUTEXES must be set to 1 in FreeRTOSConfig.h for mutexes to be available.
********************************************* Parity Inversion ********************************************************
  The potential pitfalls of using a mutex to provide mutual exclusion is called parity inversion. The sequence of
  execution depicted shows the higher priority Task 2 having to wait for the lower priority Task 1 to give up control
  of the mutex. A higher priority task being delayed by a lower priority task. This undesirable behavior would be
  exaggerated further if a medium priority task started to execute while the high priority task was waiting for the
  semaphore—the result would be a high priority task waiting for a low priority task—without the low priority task
  even being able to execute.
***********************************************************************************************************************/
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static void prvPrintTask( void *pvParameters );

static void prvNewPrintString( const char *pcString );

SemaphoreHandle_t xMutex;

const TickType_t xMaxBlockTimeTicks = 0x20;

void setup() {
  Serial.begin(9600);

  /* Before a semaphore is used it must be explicitly created.  In this example
    a mutex type semaphore is created. */
  xMutex = xSemaphoreCreateMutex();

  // Check the semaphore was created successfully.
  if (xMutex != NULL) {
    /* Create two instances of the tasks that attempt to write stdout.  The
      string they attempt to write is passed into the task as the task's
      parameter.  The tasks are created at different priorities so some
      pre-emption will occur. */
    xTaskCreate( prvPrintTask, "Print1", 100, "Task 1 ******************************************\r\n", 1, NULL );
    xTaskCreate( prvPrintTask, "Print2", 100, "Task 2 ------------------------------------------\r\n", 2, NULL );

    // Start the scheduler so the created tasks start executing.
    vTaskStartScheduler();
  }

  // The line should never be reached void loop().
  for (;;);
  return 0;
}

void loop() {
  // Empty. Things are done in Tasks.
}

static void prvNewPrintString(const char *pcString) {
  /* The semaphore is created before the scheduler is started so already
    exists by the time this task executes.

    Attempt to take the semaphore, blocking indefinitely if the mutex is not
    available immediately.  The call to xSemaphoreTake() will only return when
    the semaphore has been successfully obtained so there is no need to check the
    return value.  If any other delay period was used then the code must check
    that xSemaphoreTake() returns pdTRUE before accessing the resource (in this
    case standard out. */
  xSemaphoreTake(xMutex, portMAX_DELAY); {
    /* The following line will only execute once the semaphore has been
      successfully obtained - so standard out can be accessed freely. */
    Serial.println(pcString);
  }
  xSemaphoreGive(xMutex);

  /* Allow any key to stop the application running.  A real application that
    actually used the key value should protect access to the keyboard too.  A
    real application is very unlikely to have more than one task processing
    key presses though! */
  if (Serial.parseInt() != 0) {
    vTaskEndScheduler();
  }
}

static void prvPrintTask(void *pvParameters) {
  char *pcStringToPrint;
  const TickType_t xSlowDownDelay = pdMS_TO_TICKS(500UL);

  /* Two instances of this task are created.  The string printed by the task
    is passed into the task using the task's parameter.  The parameter is cast
    to the required type. */
  pcStringToPrint = (char *) pvParameters;

  for (;;)
  {
    Serial.println(pcStringToPrint);
    vTaskDelay( xSlowDownDelay );
  }
}
