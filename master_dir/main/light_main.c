/********************************************************************************************************
	By:				Bree Abernathy
	For:			TSL2561 Light-to-Digital sensor, for use with ESP32 with ESP-IDF
	
	Adapted from:	David Antliff
					( see https://github.com/DavidAntliff/esp32-tsl2561-example.git )
					
					
//  BREE NOTES:		I ran ESP32's I2C example prgrogram first to configure / choose 
					which of ESP32's GPIO pins to set as SDA (data) master and SCL (clock) master.
					For my board, the best choice was GPIO 18 (SDA) ad GPIO 19(SCL) but your
					board may very well be different so check the pinout in your datasheet!
					
					Also, it's important to understand the I2C is a Master-Slave connection,
					and you must wire your Master SDA and SCL pins to your Slave SDA and SCL
					pins, respectively. Finally, you also connect the sensor's SDA and SCL pins 
					to the Master SDA and SCL pins.
					
					Be sure to only connect the sensor's VCC to 3.3V! 5V will damage it.
//
//
 * MIT License
 *
 * Copyright (c) 2017 David Antliff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file app_main.c
 * @brief Example application for the TSL2561 Light-to-Digital Converter.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "../components/esp32-smbus/include/smbus.h"
#include "../components/esp32-tsl2561/include/tsl2561.h"
#include "qExample.c"
#include "fileTable.h"

#ifndef LIGHT_MAIN
#define LIGHT_MAIN

#define TAG *"app"

#define I2C_MASTER_NUM           I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN    0                     // disabled
#define I2C_MASTER_RX_BUF_LEN    0                     // disabled
#define I2C_MASTER_FREQ_HZ       100000
#define I2C_MASTER_SDA_IO        CONFIG_I2C_MASTER_SDA
#define I2C_MASTER_SCL_IO        CONFIG_I2C_MASTER_SCL



void tsl2561_output();
int lightValueRead = 0;

static void i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
}

int light_read(){
	//printf("\n |Light Read| Visible Light Value: %d\n", lightValueRead); //global variables are bad practice
	return lightValueRead;
}

void tsl2561_task(void * pvParameter)
{
    //Every 10 seconds, update the read value
    TickType_t lLastWakeTime;
    const TickType_t lFrequency = 10000*portTICK_RATE_MS;


    // Set up I2C
    i2c_master_init();
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = CONFIG_TSL2561_I2C_ADDRESS;

    // Set up the SMBus
    smbus_info_t * smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

    // Set up the TSL2561 device
    tsl2561_info_t * tsl2561_info = tsl2561_malloc();
    tsl2561_init(tsl2561_info, smbus_info);

    // Set sensor integration time and gain
    tsl2561_set_integration_time_and_gain(tsl2561_info, TSL2561_INTEGRATION_TIME_402MS, TSL2561_GAIN_1X);
    //tsl2561_set_integration_time_and_gain(tsl2561_info, TSL2561_INTEGRATION_TIME_402MS, TSL2561_GAIN_16X);

    while (1)
    {
	lLastWakeTime = xTaskGetTickCount();

        tsl2561_visible_t visible = 0;
        tsl2561_infrared_t infrared = 0;
        tsl2561_read(tsl2561_info, &visible, &infrared);
	lightValueRead = visible;
        /*printf("\nFull spectrum: %d\n", visible + infrared);
        printf("Infrared:      %d\n", infrared);
        printf("Visible:       %d\n", visible);
        printf("Lux:           %d\n", tsl2561_compute_lux(tsl2561_info, visible, infrared));*/
	light_read();
	printf("\n\n| LIGHT TASK | Before delay\n\n");	
	vTaskDelayUntil(&lLastWakeTime,lFrequency); 
	printf("\n\n| LIGHT TASK | After delay\n\n");
    }
}

int light_write(){
	int send;
	send = lightValueRead;
	send = send*10;//the "name" for light is put into the ones' place, so need to multiply the read value by 10 to make room
	send = send+2; //the name of light is 2 
	printf("| LIGHT WRITE Value sent: %d",send);
	pushQ(&send);
	//printf("\n |Light Write| Visible Light Value: %d\n", lightValueRead); //global variables are still bad practice
	return lightValueRead;
}

void light_open()
{
    xTaskCreate(&tsl2561_task, "tsl2561_task", 2048, NULL, 5, NULL);
	struct row lightRow;
	lightRow.read = light_read;
	lightRow.write = light_write;
	lightRow.name = 2;
	devTable[rowsInUse++] = lightRow;
    // I2C/SMBus Test application
    //extern void test_smbus_task(void * pvParameter);
    //xTaskCreate(&test_smbus_task, "test_smbus_task", 2048, NULL, 5, NULL);
}

void light_close(){
	vTaskSuspend(&tsl2561_task);
}

#endif
