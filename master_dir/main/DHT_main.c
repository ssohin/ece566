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
#include "fileTable.h"
#include "qExample.c"

//#define BLINK_GPIO CONFIG_BLINK_GPIO  // Bree: These are for setting via menuconfig but we don't need

//Damon: Create a task handle that's very visible to allow sensor.c to view it in the most unsafe manner possible
TaskHandle_t handleDHT = NULL;
TaskHandle_t handleHumid = NULL;
int temp_read();
int humid_read();

// Bree: define DHT22 task function
void temp_task(void *pvParameter)
{
	while(1){
		temp_read();
	}
}

void humid_task(void *pvParameter)
{
	while(1){
		humid_read();
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
	//printf( "Starting DHT Task\n\n");

	
	
		//printf("=== Reading DHT22 - Temperature (C) + Humidity Sensor ===\n\n" );
		int ret = readDHT();
		
		errorHandler(ret);
		
		//taking these two printfs out 
		//printf( "Humidity:        %.1f\n", getHumidity() );
		//printf( "Temperature (C): %.1f\n", getTemperature() );
		
		//printf("\nCounting down to next reading...\n");
		
		// Bree: I had a for loop here but it wasn't working correctly
		// Bree: this count down isn't actually counting obviously, and doesn't need to be in here
		//printf("10...\n9...\n8...\n7...\n6...\n5...\n4...\n3...\n2...\n1...\n");
		
		
		// -- wait at least 10 sec before reading again ------------
		// The interval of whole process must be beyond 10 seconds !! 
		
		// Bree: I set this higher to make it wait longer; previously it updated every second or half second
		vTaskDelay( 10000 / portTICK_RATE_MS );
		printf("\n| Temp Read | Temperature Task has this name: %d",devTable[0].name);
		return getTemperature();
}

int temp_write(){
	int ret = (int)getTemperature();
	printf("\n| Temp_Write | Temperature (C): %.1f\n", getTemperature() ); //for testing purposes, to prove that this function is being ran!
	printf("\nTemperature Task has this name on the Table: %d\n", devTable[0].name);
	pushQ(&ret);
	//printf("\nCalled Push");
	popQ();
	//printf("\nCalled Pop");
	return ret;
}

void temp_open()
{
	
	// DHT22 Read task code (from original DHT22 program)
	nvs_flash_init();
	vTaskDelay( 1000 / portTICK_RATE_MS );
	xTaskCreate( &temp_task, "TempTask", 2048, NULL, 5, &handleDHT );
	//Damon: Creates a "row" template using the temperature task, then puts it onto the table in a constant location
	struct row tempRow;
	tempRow.read = temp_read;
	tempRow.name = 1; //1 for temperature

	tempRow.write = temp_write;
	devTable[0] = tempRow;
	
	
	// LED Blink task code (from original LED Blink program)
	//xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

int humid_read(){
	setDHTgpio( 23 );    // Bree: 23 is what I chose to set my default gpio for DHT
	//printf( "Starting DHT Task\n\n");

	
	
		//printf("=== Reading DHT22 - Temperature (C) + Humidity Sensor ===\n\n" );
		int ret = readDHT();
		
		errorHandler(ret);
		
		//taking these two printfs out 
		//printf( "Humidity:        %.1f\n", getHumidity() );
		//printf( "Temperature (C): %.1f\n", getTemperature() );
		
		//printf("\nCounting down to next reading...\n");
		
		// Bree: I had a for loop here but it wasn't working correctly
		// Bree: this count down isn't actually counting obviously, and doesn't need to be in here
		//printf("10...\n9...\n8...\n7...\n6...\n5...\n4...\n3...\n2...\n1...\n");
		
		
		// -- wait at least 10 sec before reading again ------------
		// The interval of whole process must be beyond 10 seconds !! 
		
		// Bree: I set this higher to make it wait longer; previously it updated every second or half second
		vTaskDelay( 10000 / portTICK_RATE_MS );
		printf("\n| Humid_Read | Humidity Task has this name: %d",devTable[1].name);
		return getHumidity();
}

int humid_write(){
	int ret = (int)getTemperature();
	printf("\n| Humid_Write | Humidity: %.1f\n", getHumidity()); //for testing purposes, to prove that this function is being ran!
	printf("\nHumidity Task has this name: %d\n" ,devTable[1].name);
	return ret;
}

void humid_open()
{
	// DHT22 Read task code (from original DHT22 program)
	nvs_flash_init();
	vTaskDelay( 1000 / portTICK_RATE_MS );
	//Damon: These task definitions mean it only looks at humidity side of things
	xTaskCreate( &humid_task, "HumidTask", 2048, NULL, 5, &handleHumid );
	//Damon: Creates a "row" template using the temperature task, then puts it onto the table in a constant location
	struct row humidRow;
	humidRow.read = humid_read;
	humidRow.name = 2; //2 for humidity

	humidRow.write = humid_write;
	devTable[1] = humidRow; //row value 1 is humid
}
