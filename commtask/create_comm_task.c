#include "communications.c"

// Buffer that the task being created will use as its stack.  Note this is
// an array of StackType_t variables.  The size of StackType_t is dependent on
// the RTOS port.

//Damon:I don't know how much stack DHT will need so this should go up/down as needed for different task implementations
int STACK_SIZE = 63;

StackType_t xCommStack[ STACK_SIZE ];

// Damon: These all start with the same priority I think, but maybe the ctl command is supposed to change that so I made it a variable
int Comm_priority = 10; 

// Damon: Function that creates a task. Each task needs its own unique creation function I think, unless we want to mess around with adding parameters to these.
// Damon: From what I understand, Manager task should create/destroy tasks as needed, and this is an example that should create a task using DHT_task()
void createCommTask( void ) 
{
    TaskHandle_t xCommHandle = NULL;

    // Create the task without using any dynamic memory allocation.
	//Damon: This can be ported to the dynamic-memory task creation function if you guys want, I just chose the simpler one
    xHandle = xTaskCreateStatic(
                  Comm_task,       // Function that implements the task.
                  "Comm_Name",          // Text name for the task.
                  STACK_SIZE,      // Stack size in bytes, not words.
                  ( void * ) 1,    // Parameter passed into the task.
                  Comm_priority,		// Priority at which the task is created.
                  xCommStack,          // Array to use as the task's stack.
                  &xTaskBuffer );  // Variable to hold the task's data structure.

    // puxStackBuffer and pxTaskBuffer were not NULL, so the task will have
    // been created, and xHandle will be the task's handle.  Use the handle
    // to suspend the task.
    vTaskSuspend( xCommHandle );
}