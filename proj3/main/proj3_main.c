#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "sensors.h"
#include "DHT11.h"



/*
The Sensor class has 3 pure virtual functions and a virtual function close.
Each new sensor can be inherited from the class Sensor and can use it's configurations for
Open, Read, Control and Close.
*/
class Sensor {
public:
    virtual void open() = 0;
    virtual void read() = 0;
    virtual void close()
    {
    public:
        int AllSensorData[10] = [ 0,0,0,0,0,0,0,0,0,0]
           // [DHT_temp, DHT Humidity, PhotoSensor, ] 
    }
    virtual void ctl() = 0;
};

/*
Sensor Definition Template:
--------------------------------------
--------------------------------------

class sensorname : public Sensor
{
 public:


    int open(gpioPIN) {

    }
    void read() {

    }
    void close() {

    }
    void ctl() {

    }
};

----------------------------------------
----------------------------------------
*/



class DHT11 : public Sensor
{
 public:
     int DHT_temp = 0;
     int DHT_humidity = 0;
    void open()
    {
        setDHTPin(4);
        printf("\n DHT Open\n");
    }
    void read()
    {
        printf("Temperature reading %d\n", getTemp());
        printf("F temperature is %d\n", getFtemp());
        printf("Humidity reading %d\n", getHumidity());
        vTaskDelay(3000 / portTICK_RATE_MS);
        DHT_temp = getTemp();
        DHT_humidity = getHumidity();
    }
    void close()
    {

    }
    void ctl()
    {

    }
};



void DHT_Task(void *pvParameter)
{
    DHT11 d;
    d.open();
    d.read();
    d.close();
}

void Sensing_Task(void *pvParameter)
{

}

void Manager_Task(void *pvParameter)
{

}

void app_main
{
    nvs_flash_init();
    system_init();
    vTaskDelay(1000 / portTICK_RATE_MS);
    xTaskCreate(&DHT_Task, "DHT_Task", 2048, NULL, 5, NULL);
    xTaskCreate(&Sensing_Task, "Sensing_Task", 2048, NULL, 5, NULL)
    xTaskCreate(&Manager_Task, "Manager_Task", 2048, NULL, 5, NULL)

}

