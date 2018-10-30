/*
Adapted from Bree's DHT22 Code:
Home Environment Monitor Sensor Drivers
*/

#ifndef DHT22_H_  
#define DHT22_H_
#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2
//Add more sensors here

// == function prototypes =======================================

void 	setDHTgpio(int gpio);
void    setLEDgpio(int gpio); // Bree created to integrate her LED blinking for fun

void 	errorHandler(int response);
int 	readDHT();
float 	getHumidity();
float 	getTemperature();
int 	getSignalLevel(int usTimeOut, bool state);

#endif
