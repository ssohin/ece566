#include "DHT_main.c"
#include "light_main.c"
#include "qExample.c"
#include "sensor.c"
#include "fileTable.h"
#include "communications.c"

TaskHandle_t handleManage = NULL;
void manage_task();
int tempAvg;
int humidAvg;
int lightAvg;
int sort; //sort looks at the first digit off of top of queue, then sends it to correct averaging value

void app_main(){
	temp_open();
	humid_open(); //open all of the tasks we'll need and put them on the stack. We implemented it this way to make the Mnager more modular, but same idea as example code in class.
	light_open();
	q_app_main();
	sense_open();
	comms_open();
	tempAvg = temp_write()/10;
	humidAvg = humid_write()/10;
	lightAvg = light_write()/10;
	nvs_flash_init();
	vTaskDelay( 1000 / portTICK_RATE_MS );
	xTaskCreate( &manage_task, "ManageTask", 2048, NULL, 5, &handleManage );
}

void updateTempAvg(int toAdd){
	toAdd = toAdd/10;
	tempAvg = 8*tempAvg; //weighted average: 80% current value, 20% new value
	toAdd = 2*toAdd;
	tempAvg += toAdd;
	tempAvg = tempAvg/10; 

	toComm = (tempAvg*10)+0; //remove the value in the ones' place so communication task can see the name of this sensor
	printf("\nWeighted Temperature Average: %d\n",tempAvg);
}

void updateHumidAvg(int toAdd){
	
	toAdd = toAdd/10;
	humidAvg = 8*humidAvg; //weighted average: 80% current value, 20% new value
	toAdd = 2*toAdd;
	humidAvg += toAdd;
	humidAvg = humidAvg/10;

	toComm = (humidAvg*10)+1; //shift the humidity average by 10 so communications task can see the name of this sensor
	printf("\nWeighted Humidity Average: %d\n",humidAvg);

}

void updateLightAvg(int toAdd){

	toAdd = toAdd/10;
	lightAvg = 8*lightAvg; //weighted average: 80% current value, 20% new value
	toAdd = 2*toAdd;
	lightAvg += toAdd;
	lightAvg = lightAvg/10;

	toComm = (lightAvg*10)+2; //shift the light average by 10 so communications task can see the name of this sensor
	printf("\nWeighted Light Average: %d\n",lightAvg);
}

void noUpdates(){
	printf("\nNo values found on queue this period.\n");
}

void manage_task(){
	int name;
	int tableIndex;

	TickType_t xLastWakeTime;
	//every ten seconds, read off from the queue
	const TickType_t xFrequency = 10000*portTICK_RATE_MS;
	while(1){
	//to test that the functions are working properly
	xLastWakeTime = xTaskGetTickCount();
	
	for(tableIndex = 0; tableIndex < rowsInUse; tableIndex++){		
	sort = popQ();
	name = sort%10; //name is stored in ones' place of sort, so this works I think

	switch(name){
		case 0:
			updateTempAvg(sort);
			break;
		case 1:
			updateHumidAvg(sort);
			break;
		case 2: 
			updateLightAvg(sort);
			break;
		default:
			noUpdates();
			break;

			}//end switch case
		
		vTaskDelayUntil(&xLastWakeTime,xFrequency);
		
		}//end tableIndexing
	}//end whileLoop for manager task
}//end manager task
