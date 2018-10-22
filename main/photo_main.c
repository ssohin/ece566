/*
ECE-566 RTOS - FALL 2018
Adapted by Damon Myers

For: Photosensitivity sensor 

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "photo.h"



// Damon: define photosensitivity task function
void photo_task(void *pvParameter)
{
	setPhotogpio( 14 );    // Damon: I used 14 as my default pin, can change to whatever is available
	printf( "Starting Photo Task\n\n");

	while(1) {
	
		printf("=== Reading Photosensitivty sensor ===");
		int ret = readPhoto();
		
		errorHandler(ret);
		
		printf( "Light Level:        %.1f\n", getLight() );
		
		printf("\nCounting down to next reading...\n");
		
		// Bree: I had a for loop here but it wasn't working correctly
		// Bree: this count down isn't actually counting obviously, and doesn't need to be in here
		printf("10...\n9...\n8...\n7...\n6...\n5...\n4...\n3...\n2...\n1...\n");
		
		
		// -- wait at least 10 sec before reading again ------------
		// The interval of whole process must be beyond 10 seconds !! 
		
		// Bree: I set this higher to make it wait longer; previously it updated every second or half second
		vTaskDelay( 10000 / portTICK_RATE_MS );
	}
}


	//Bree: define LED Blink task function
// void blink_task(void *pvParameter)
// {
    // /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       // muxed to GPIO on reset already, but some default to other
       // functions and need to be switched to GPIO. Consult the
       // Technical Reference for a list of pads and their default
       // functions.)
    // */
		//gpio_pad_select_gpio(LEDgpio);
	// setLEDgpio( 2 );
    // /* Set the GPIO as a push/pull output */
    // gpio_set_direction(2, GPIO_MODE_OUTPUT);
    // while(1) {
        // /* Blink off (output low) */
        // gpio_set_level(2, 0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // /* Blink on (output high) */
        // gpio_set_level(2, 1);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
// }


void app_main()
{
	// Photo Read task code (from original DHT22 program)
	nvs_flash_init();
	vTaskDelay( 1000 / portTICK_RATE_MS );
	xTaskCreate( &Photo_task, "Photo_task", 2048, NULL, 5, NULL );
	
	/*
	// LED Blink task code (from original LED Blink program)
	xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
	*/
	
}

