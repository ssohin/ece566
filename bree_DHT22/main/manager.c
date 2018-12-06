#include "DHT_main.c"
#include "light_main.c"
#include "qExample.c"
#include "sensor.c"
#include "fileTable.h"
#include "communications.c"
#include "command.c"

TaskHandle_t handleManage = NULL;
void manage_task();
int tempAvg;
int humidAvg;
int lightAvg;

gpio_num_t SUPERUSER_GPIO = GPIO_NUM_27; //variables to make them easier to change
char input[20];
char password[] = "TestPassword";

int sort; //sort looks at the first digit off of top of queue, then sends it to correct averaging value

void app_main(){
	
	temp_open();
	humid_open(); //open all of the tasks we'll need and put them on the stack. We implemented it this way to make the Mnager more modular, but same idea as example code in class.
	light_open();
	
	q_app_main(); //these are data structures used by the manager to control the rest of the system.
	command_open();
	sense_open();
	comms_open();
	command_open();
	
	tempAvg = temp_write()/10; //then set initial values
	humidAvg = humid_write()/10;
	lightAvg = light_write()/10;
	nvs_flash_init();
	xTaskCreate( &manage_task, "ManageTask", 2048, NULL, 5, &handleManage );
}

void updateTempAvg(int toAdd){
	int tempTempAvg;
	toAdd = toAdd/10;
	tempAvg = 6*tempAvg; //weighted average: 60% current value, 40% new value
	toAdd = 4*toAdd;
	tempAvg += toAdd;
	tempAvg = tempAvg/10;
       	tempTempAvg = tempAvg*10;
	pushW(&tempTempAvg);
//	printf("\nWeighted Temperature Average: %d\n",tempAvg);
	
	//set temperature's signature value for command task
	if(tempAvg > 40){
		signature[0] = 2; //probably fire
	}else if(tempAvg > 25){
		signature[0] = 1; //hot summer day
	}else if(tempAvg > 20){
		signature[0] = 0; //20-25C is about room temperature
	}else if(tempAvg > 15){
		signature[0] = -1; //cold winter day outside
	}else
		signature[0] = -2; //terrifying apocalyptic event
	
}

void updateHumidAvg(int toAdd){
	int tempHumidAvg;
	toAdd = toAdd/10;
	humidAvg = 6*humidAvg; //weighted average: 60% current value, 40% new value
	toAdd = 4*toAdd;
	humidAvg += toAdd;
	humidAvg = humidAvg/10;
	tempHumidAvg = (humidAvg*10)+1;
	pushW(&tempHumidAvg);;
//	printf("\nWeighted Humidity Average: %d\n",humidAvg);
	
	if(humidAvg > 50){
		signature[1] = 2; //way too humid
	}else if(humidAvg > 40){
		signature[1] = 1; //still too humid but a believable amount
	}else if(humidAvg > 25){
		signature[1] = 0; //20-25C is about room temperature
	}else if(humidAvg > 15){
		signature[1] = -1; //dry day
	}else
		signature[1] = -2; //terrifying apocalyptic event

}

void updateLightAvg(int toAdd){
	
	int tempLightAvg;
	toAdd = toAdd/10;
	lightAvg = 6*(lightAvg)*(lightAvg); //weighted average: 60% current value, 40% new value
	toAdd = 4*toAdd;
	lightAvg += toAdd;
	lightAvg = lightAvg/10;
	tempLightAvg = (lightAvg*10)+2;
	pushW(&tempLightAvg);	
//	printf("\nWeighted Light Average: %d\n",lightAvg);
	
	if(lightAvg > 400){
		signature[2] = 2; //possible that the sun exploded
	}else if(lightAvg > 300){
		signature[2] = 1; //still too bright
	}else if(lightAvg > 200){
		signature[2] = 0; //I think this is normal? Need to calibrate this
	}else if(lightAvg > 100){
		signature[2] = -1; //normal night darkness
	}else
		signature[2] = -2; //terrifying apocalyptic event
}

void noUpdates(){
	printf("\nNo values found on queue this period.\n");
}

void manage_task(){
	int name; //name is the "name" of the sensor, put into the ones place for ease of reading
	int tableIndex;

	TickType_t xLastWakeTime;
	//every ten seconds, read off from the queue
	const TickType_t xFrequency = 15000*portTICK_RATE_MS;
	while(1){
	//to test that the functions are working properly
	xLastWakeTime = xTaskGetTickCount();
	
	for(tableIndex = 0; tableIndex < rowsInUse; tableIndex++){		
	sort = popQ();
	printf("\nManager Task: %d is popQ\n", sort);
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
	
	


				
		}//end tableIndexing
	
		vTaskDelayUntil(&xLastWakeTime,xFrequency);

	}//end whileLoop for manager task
}//end manager task
