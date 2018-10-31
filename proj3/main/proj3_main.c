#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "sensors.h"


/*
The Sensor class has 4 pure virtual functions
Each new sensor can be inherited from the class Sensor and can use it's configurations for
Open, Read, Control and Close.
*/
class Sensor {
public:
    virtual void open() = 0;
    virtual void read() = 0;
    virtual void close() = 0;
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
voidapp_main
{
   
    
}

