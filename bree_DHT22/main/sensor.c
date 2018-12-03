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
	int tableIndex;
		
		TickType_t sLastWakeTime;
		const TickType_t sFrequency = 10000*portTICK_RATE_MS;
	while(1){
		//sensor task just calls the write() functions, which push read values onto the shared queue
		sLastWakeTime = xTaskGetTickCount();
		for(tableIndex = 0; tableIndex < rowsInUse; tableIndex++){
			printf("\n|SENSE TASK| tableIndex = %d and read value is %d ", tableIndex, devTable[tableIndex].write()/10);
		}
	
	vTaskDelayUntil(&sLastWakeTime, sFrequency);
	}
}
