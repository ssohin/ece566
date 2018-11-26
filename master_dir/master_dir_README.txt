=====================================================================================================
/* DHT22 Temp & Humidity Sensor + LED Blink */

By:		Bree Abernathy
For:	ECE566 RTOS - Fall 2018
=====================================================================================================
DHT22 / AM2302 + LED -- ESP-IDF library and code for ESP32
=====================================================================================================
	Code based on example programs and libraries for GPIO and
	DHT22 sensor developed by Ricardo Timmermann, Sam Johnston,
	Markus Ulsass, Adafruit Industries, and Espressif.
	
	This program combines basic DHT22 temperature and humidity readings with LED Blink via
	user - customizable GPIO pin configuration.
	Defaults in code are:
		GPIO pin 23 = DHT22
		GPIO pin  2 = LED
		
	DHT reading and LED blinking periodicities can be defined via taskdelay functions. Please see
	FreeRTOS.h and freeRTOS/task.h and supplemental documentation found in ESP-IDF for details.
=====================================================================================================	
****For independent DHT22 code:

//	Create folder called DTH22. In this folder run:
		$ git clone https://github.com/gosouth/DHT22.git 
//	See: DHT22_README.md & DHT22_README.rst

****For independent LED Blink code:

//	Create folder called DTH22. In this folder run:
		$ git clone https://github.com/markbeee/ESP32_Examples/tree/master/blink
// See: Blink README at https://github.com/markbeee/ESP32_Examples/blob/master/blink/Readme.md
=====================================================================================================