#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "sensors.h"
#include "string.h"

class Sensor
{
public:
    string SensorName;
    int gpio;
    template SensorData()

        Open()
        Ctl()
        Read()
        Close()
};

class TempSensor :pubic Sensor
{
    
};
