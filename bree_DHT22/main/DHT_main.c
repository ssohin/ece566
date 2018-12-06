/*
ECE-566 RTOS - FALL 2018
Adapted by Bree Abernathy

For: DHT22 Temp & Humidity Sensor + LED  

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "DHT22.h"
#include "qExample.c"
#include "fileTable.h"

#ifndef DHT_MAIN
#define DHT_MAIN

//#define BLINK_GPIO CONFIG_BLINK_GPIO  // Bree: These are for setting via menuconfig but we don't need

//Damon: Create a task handle that's very visible to allow sensor.c to view it in the most unsafe manner possible
TaskHandle_t handleDHT = NULL;
TaskHandle_t handleHumid = NULL;



int temp_read();
int humid_read();

int tempValue = 0; //read updates these values, write puts them onto the queue
int humidValue = 0;

float SSt = 0; //to find the standard deviation of temperature task, take sum of values and sum of squared values, then divide by n
float St = 0;
int nt = 0;

float SSh = 0;
float Sh = 0;
int nh = 0;

int TEMP_TASK_PERIOD = 10000;
int HUMID_TASK_PERIOD = 10000; 

// Bree: define DHT22 task function
void temp_task(void *pvParameter)
{
		//define values used in vTaskDelayUntil() to make task periodic
		TickType_t xLastWakeTime;
		TickType_t x2DoneWithTask;
		const TickType_t xFrequency = TEMP_TASK_PERIOD*portTICK_RATE_MS;

	while(1){
		xLastWakeTime = xTaskGetTickCount();

		temp_read(); //temp_task periodically updates the value
		
		//For dev use only. Used in determining task characteristics
		x2DoneWithTask = xTaskGetTickCount();
		
		
		St += x2DoneWithTask - xLastWakeTime;
		SSt += (x2DoneWithTask - xLastWakeTime)*(x2DoneWithTask - xLastWakeTime);
		nt++;
		
		printf("\n\nTemp Task Info: %d Ticks to complete task",(x2DoneWithTask - xLastWakeTime));
		printf("\nTemp Task Info: %f is Standard Deviation", ((SSt/nt) - (St/nt)*(St/nt))); //this is 0 because DHT22 takes 3 ticks to read by design
		printf("\nTemp Task Info: %d is Temperature Slack Time", 100 - (x2DoneWithTask - xLastWakeTime));
		printf("\nTemp Task Info: %d is utilization", (x2DoneWithTask - xLastWakeTime)/TEMP_TASK_PERIOD);
		printf("\nTemp Task Info: %d is Task Period", TEMP_TASK_PERIOD);
		 //
		
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		
	
	}

}

void humid_task(void *pvParameter)
{
	//see temp_task for explanation of periodic task control
	TickType_t hLastWakeTime;
	TickType_t h2DoneWithTask;
	const TickType_t hFrequency = 10000*portTICK_RATE_MS;
	while(1){
		hLastWakeTime = xTaskGetTickCount();
		humid_read(); //temp_task periodically updates the value
		
		//For dev use only. Used in determining task characteristics
		h2DoneWithTask = xTaskGetTickCount();
		
		
		Sh += h2DoneWithTask - hLastWakeTime;
		SSh += (h2DoneWithTask - hLastWakeTime)*(h2DoneWithTask - hLastWakeTime);
		nh++;
		
		printf("\n\nTemp Task Info: %d Ticks to complete task",(h2DoneWithTask - hLastWakeTime));
		printf("\nTemp Task Info: %f is Standard Deviation", ((SSh/nh) - (Sh/nh)*(Sh/nh))); //this is 0 because DHT22 takes 3 ticks to read by design
		printf("\nTemp Task Info: %d is Temperature Slack Time", 100 - (h2DoneWithTask - hLastWakeTime));
		printf("\nTemp Task Info: %d is utilization", (h2DoneWithTask - hLastWakeTime)/HUMID_TASK_PERIOD);
		printf("\nTemp Task Info: %d is Task Period", HUMID_TASK_PERIOD);
		 //
		
		vTaskDelayUntil(&hLastWakeTime, hFrequency);
		

	}
}

//Bree: define LED Blink task function
void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    //gpio_pad_select_gpio(LEDgpio);
	setLEDgpio( 2 );
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(2, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(2, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(2, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

int temp_read(){
	setDHTgpio( 23 );    // Bree: 23 is what I chose to set my default gpio for DHT
	
		int ret = readDHT();
		
		errorHandler(ret);
		
		tempValue = getTemperature();
		return getTemperature();
}

void temp_ioctl(int input){ //change values of temp
	TEMP_TASK_PERIOD = input;
}

int temp_write(){
	int ret = tempValue;
	ret = ret*10; //the "name" for temperature is put into the ones' place, so value needs to be multiplied by 10 to make room
	ret = ret+0; //name for temperature is 0
	pushQ(&ret);
	return ret;
}

void humid_ioctl(int input){
	HUMID_TASK_PERIOD = input;
}

void humid_close(){
	vTaskSuspend(handleHumid);
}

void temp_close(){
	vTaskSuspend(handleDHT);
}

void temp_open()
{
	if(handleDHT == NULL){	//on first call, define this task
	// DHT22 Read task code (from original DHT22 program)
	nvs_flash_init();
	vTaskDelay( 1000 / portTICK_RATE_MS );
	xTaskCreate( &temp_task, "TempTask", 2048, NULL, 5, &handleDHT );
	struct row tempRow;
	tempRow.read = temp_read;
	tempRow.name = 1;
	tempRow.write = temp_write;
	tempRow.close = temp_close;
	tempRow.open = temp_open;
	devTable[rowsInUse++] = tempRow;
	}else{ //on every other call, resume this task
		vTaskResume(handleDHT);
	}
	// LED Blink task code (from original LED Blink program)
	//xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

int humid_read(){
	setDHTgpio( 23 );    // Bree: 23 is what I chose to set my default gpio for DHT
		
		int ret = readDHT();
		errorHandler(ret);
		
		//printf("\n| Humid_Read | Humidity Task has this name: %d",devTable[1].name);
		return getHumidity();
}

int humid_write(){

     	// Initialise the xLastWakeTime variable with the current time.
	
     	int ret = (int)getHumidity();
	ret = ret*10; //the "name" for humidity is put into the ones place of the read value
	ret += 1; //humidity = 1
	pushQ(&ret); //puts humidity value onto queue
	return ret;
}

void humid_open()
{
	if(handleHumid == NULL){ //on first call, define this task
	// DHT22 Read task code (from original DHT22 program)
	nvs_flash_init();
	vTaskDelay( 1000 / portTICK_RATE_MS );
	//Damon: These task definitions mean it only looks at humidity side of things
	xTaskCreate( &humid_task, "HumidTask", 2048, NULL, 5, &handleHumid );
	struct row humidRow;
	humidRow.read = humid_read;
	humidRow.write = humid_write;
	humidRow.open = humid_open;
	humidRow.close = humid_close;
	humidRow.name = 2;
	devTable[rowsInUse++] = humidRow;
	}else{ //on every other call, resume this task
		vTaskResume(handleHumid);
		
	}
}
#endif
