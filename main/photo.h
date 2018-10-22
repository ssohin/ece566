/* 

	Photosensitive Sensor: Voltage value changes based on light

*/

#ifndef PHOTO_H_  
#define PHOTO_H_

#define PHOTO_OK 0
#define PHOTO_CHECKSUM_ERROR -1
#define PHOTO_TIMEOUT_ERROR -2

// == function prototypes =======================================

void 	setPhotogpio(int gpio);
void 	errorHandler(int response);
int 	readPhoto();
float 	getLight();
int 	getSignalLevel( int usTimeOut, bool state );

#endif
