/* 

	DHT22 temperature sensor driver

*/

#ifndef DHT22_H_  
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2


//	lines 17-37 borrowed from dht11.h 
/*
#ifndef DHT22_H_  
#define DHT22_H_

#include "driver/gpio.h"

enum dht22_status {
	DHT22_CRC_ERROR = -2,
	DHT22_TIMEOUT_ERROR,
	DHT22_OK
};

struct dht22_reading {
	int status;
	int temperature;
	int humidity;
};

void DHT22_init(gpio_num_t);
struct dht22_reading DHT22_read();

#endif
*/

// == function prototypes =======================================

void 	setDHTgpio(int gpio);
void    setLEDgpio(int gpio); // Bree created to integrate her LED blinking for fun
void 	errorHandler(int response);
int 	readDHT();
float 	getHumidity();
float 	getTemperature();
int 	getSignalLevel( int usTimeOut, bool state );

#endif
