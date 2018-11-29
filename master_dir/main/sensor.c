#include "DHT_main.c"
#include "light_main.c"
#include "qExample.c"

TaskHandle_t handleSense = NULL;
void sense_task();

void sense_open(){
	nvs_flash_init();
	vTaskDelay( 1000 / portTICK_RATE_MS );
	xTaskCreate( &sense_task, "SenseTask", 2048, NULL, 5, &handleSense );
}

void sense_task(){
	while(1){
	//sensor task just calls the write() functions, which push read values onto the shared queue
	temp_write();
	light_write();
	humid_write();
	
	vTaskDelay(1000/portTICK_RATE_MS);	
	}
}
