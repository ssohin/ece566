#include "DHT_main.c"

void init_DHT(){
	DHT_app_main(); //creates task handleDHT and suspends it until sensor resumes it temporarily	
}

void read_DHT(){
	vTaskResume(&handleDHT);
	vTaskDelay( 10000 / portTICK_RATE_MS ); //should change this based on some kind of other value
	vTaskSuspend(&handleDHT);
}