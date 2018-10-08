//Example static (known stack size) task creation:

	// Dimensions the buffer that the task being created will use as its stack.
	// NOTE:  This is the number of words the stack will hold, not the number of
	// bytes.  For example, if each stack item is 32-bits, and this is set to 100,
	// then 400 bytes (100 * 32-bits) will be allocated.
	#define STACK_SIZE 200

	// Structure that will hold the TCB of the task being created.
	StaticTask_t xTaskBuffer;

	// Buffer that the task being created will use as its stack.  Note this is
	// an array of StackType_t variables.  The size of StackType_t is dependent on
	// the RTOS port.
	StackType_t xStack[ STACK_SIZE ];

	// Function that implements the task being created.
	void vTaskCode( void * pvParameters )
	{
		// The parameter value is expected to be 1 as 1 is passed in the
		// pvParameters value in the call to xTaskCreateStatic().
		configASSERT( ( uint32_t ) pvParameters == 1UL );

		for( ;; )
		{
			// Task code goes here.
		}
	}

	// Function that creates a task.
	void vOtherFunction( void )
	{
		TaskHandle_t xHandle = NULL;

		// Create the task without using any dynamic memory allocation.
		xHandle = xTaskCreateStatic(
					  vTaskCode,       // Function that implements the task.
					  "NAME",          // Text name for the task.
					  STACK_SIZE,      // Stack size in words, not bytes.
					  ( void * ) 1,    // Parameter passed into the task.
					  tskIDLE_PRIORITY,// Priority at which the task is created.
					  xStack,          // Array to use as the task's stack.
					  &xTaskBuffer );  // Variable to hold the task's data structure.

		// puxStackBuffer and pxTaskBuffer were not NULL, so the task will have
		// been created, and xHandle will be the task's handle.  Use the handle
		// to suspend the task.
		
		vTaskSuspend( xHandle );
		//vTaskResume( xHandle ); to resume the task and it will run in accordance to its priority
		//INCLUDE_vTaskSuspend must be set to 1 otherwise can't supsend and resume tasks
		
		
	}
	
	

//relative delay for a task
void vTaskFunction( void * pvParameters )
{
// Block for 500ms.
const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    for( ;; )
    {
        // Simply toggle the LED every 500ms, blocking between each toggle.
        vToggleLED();
        vTaskDelay( xDelay );
    }
}


//takes taskhandle and gives the human-readable task name
char *pcTaskGetTaskName(TaskHandle_t xTaskToQuery)


//returns start of stack for xTask
uint8_t *pxTaskGetStackStart(TaskHandle_t xTask) //use NULL for xTask to return start stack of current task

