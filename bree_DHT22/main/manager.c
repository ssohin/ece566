#include "sensor.c"
#include "queue.h" //includes a lot of tools for creating a queue structure

Queuehandle_t dhtq; //creates a queue data type


void manager_task(){
	while(1){
		
		
	}
}

void app_main(){
	dhtq = xQueueCreate(10,sizeof(float)); //now holds 10 flaots, which will be the temperature values probably
	initDHT(); //initialize dht task
	xTaskCreate( &manager_task, "manager_task", 2048, NULL, 5, &handleManager );
}