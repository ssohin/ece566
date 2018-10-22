/*------------------------------------------------------------------------------

	Likely horrible photosensitivity sensor code, built off of:
	DHT22 temperature & humidity sensor AM2302 (DHT22) driver for ESP32

	Jun 2017:	Ricardo Timmermann, new for DHT22  	

	Code Based on Adafruit Industries and Sam Johnston and Coffe & Beer. Please help
	to improve this code. -Damon: I don't even have the skill to make the code work as good as it did before
	
	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.

	PLEASE KEEP THIS CODE IN LESS THAN 0XFF LINES. EACH LINE MAY CONTAIN ONE BUG !!!

---------------------------------------------------------------------------------*/

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "photo.h"

// == global defines =============================================

static const char* TAG = "PHOTO";

int Photogpio = 14;				// Damon's default photo gpio pin = 14
float light = 0.0; 				//initialize to 0. 
								//According to documentation of this generic light sensor, 512 is used as the border for "HIGH" value but I don't know if I trust that
								
// == set the photosensitive user pin=========================================

void setPhotogpio( int gpio ) {	Photogpio = gpio; } //call if you want to change Photogpio to a non-default value without changing this code

// == get light =============================================

float getLight() { return light; }

// == error handler ===============================================

void errorHandler(int response)
{
	switch(response) {
	
		case PHOTO_TIMEOUT_ERROR :
			ESP_LOGE( TAG, "Sensor Timeout\n" );
			break;

		case PHOTO_CHECKSUM_ERROR:
			ESP_LOGE( TAG, "CheckSum error\n" );
			break;

		case PHOTO_OK:
			break;

		default :
			ESP_LOGE( TAG, "Unknown error\n" );
	}
}

/*-------------------------------------------------------------------------------
;
;	get next state 
;
;	I don't like this logic. It needs some interrupt blocking / priority
;	to ensure it runs in realtime.
;
;--------------------------------------------------------------------------------*/

int getSignalLevel( int usTimeOut, bool state )
{

	int uSec = 0;
	while( gpio_get_level(Photogpio)==state ) { //returns voltage(?) of the pin associated with photosensitivity sensor, by default is 14

		if( uSec > usTimeOut ) 
			return -1;
		
		++uSec;
		ets_delay_us(1);		// uSec delay
	}
	
	return uSec;
}

/*----------------------------------------------------------------------------
;
;	read DHT22 sensor

copy/paste from AM2302/DHT22 Docu:

DATA: Hum = 16 bits, Temp = 16 Bits, check-sum = 8 Bits

Example: MCU has received 40 bits data from AM2302 as
0000 0010 1000 1100 0000 0001 0101 1111 1110 1110
16 bits RH data + 16 bits T data + check sum

1) we convert 16 bits RH data from binary system to decimal system, 0000 0010 1000 1100 → 652
Binary system Decimal system: RH=652/10=65.2%RH

2) we convert 16 bits T data from binary system to decimal system, 0000 0001 0101 1111 → 351
Binary system Decimal system: T=351/10=35.1°C

When highest bit of temperature is 1, it means the temperature is below 0 degree Celsius. 
Example: 1000 0000 0110 0101, T= minus 10.1°C: 16 bits T data

3) Check Sum=0000 0010+1000 1100+0000 0001+0101 1111=1110 1110 Check-sum=the last 8 bits of Sum=11101110

Signal & Timings:

The interval of whole process must be beyond 2 seconds.

To request data from DHT:

1) Sent low pulse for > 1~10 ms (MILI SEC)
2) Sent high pulse for > 20~40 us (Micros).
3) When DHT detects the start signal, it will pull low the bus 80us as response signal, 
   then the DHT pulls up 80us for preparation to send data.
4) When DHT is sending data to MCU, every bit's transmission begin with low-voltage-level that last 50us, 
   the following high-voltage-level signal's length decide the bit is "1" or "0".
	0: 26~28 us
	1: 70 us

;----------------------------------------------------------------------------*/

#define MAXPhotoData 3	// to complete 24 = 3*8 Bits

int readPhoto()
{
int uSec = 0;

uint8_t PhotoData[MAXPhotoData];
uint8_t byteInx = 0;
uint8_t bitInx = 7;

	for (int k = 0; k<MAXPhotoData; k++) 
		PhotoData[k] = 0;

	// == Send start signal to photo sensor (? Don't know if photo sensor also needs a start signal) ===========

	gpio_set_direction( Photogpio, GPIO_MODE_OUTPUT );

	// pull down for 3 ms for a smooth and nice wake up 
	gpio_set_level( Photogpio, 0 );
	ets_delay_us( 3000 );			

	// pull up for 25 us for a gentile asking for data
	gpio_set_level( Photogpio, 1 );
	ets_delay_us( 25 );

	gpio_set_direction( Photogpio, GPIO_MODE_INPUT );		// change to input mode
  
	// == DHT will keep the line low for 80 us and then high for 80us ====

	uSec = getSignalLevel( 85, 0 ); //timeout is for 85 usec, state set to 0
//	ESP_LOGI( TAG, "Response = %d", uSec );
	if( uSec<0 ) return PHOTO_TIMEOUT_ERROR; 

	// -- 80us up ------------------------

	uSec = getSignalLevel( 85, 1 ); //timeout 85 usec, state set to 1
//	ESP_LOGI( TAG, "Response = %d", uSec );
	if( uSec<0 ) return PHOTO_TIMEOUT_ERROR;

	// == No errors, read the 24*?) data bits ================
  
	for( int k = 0; k < 24; k++ ) {

		// -- starts new data transmission with >50us low signal

		uSec = getSignalLevel( 56, 0 );
		if( uSec<0 ) return PHOTO_TIMEOUT_ERROR;

		// -- check to see if after >70us rx data is a 0 or a 1

		uSec = getSignalLevel( 75, 1 );
		if( uSec<0 ) return PHOTO_TIMEOUT_ERROR;

		// add the current read to the output data
		// since all dhtData array where set to 0 at the start, 
		// only look for "1" (>28us us)
	
		if (uSec > 24) {
			PhotoData[ byteInx ] |= (1 << bitInx);
			}
	
		// index to next byte

		if (bitInx == 0) { bitInx = 7; ++byteInx; }
		else bitInx--;
	}

	// == get light from Data[0] and Data[1] ==========================

	light = PhotoData[0];
	light *= 0x100;					// >> 8
	light += PhotoData[1];
	light /= 10;						// get the decimal

/*	// == get temp from Data[2] and Data[3] //////////////////////////////////Commented out due to photo sensor only detecting one value as opposed to DHT which outputs two
	
	temperature = dhtData[2] & 0x7F;	
	temperature *= 0x100;				// >> 8
	temperature += dhtData[3];
	temperature /= 10;

	if( dhtData[2] & 0x80 ) 			// negative temp, brrr it's freezing
		temperature *= -1;

*/
	// == verify if checksum is ok ===========================================
	// Checksum is the sum of Data 8 bits masked out 0xFF
	/*
	if (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF)) 
		return DHT_OK;

	else 
		return DHT_CHECKSUM_ERROR;
	*/
	
	if(PhotoData[3] == ((PhotoData[0] + PhotoData[1]) & 0xFF))
		return PHOTO_OK;
	else
		return PHOTO_CHECKSUM_ERROR;
		
}

