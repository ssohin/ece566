#include <string.h>
#include <stdlib.h>

#include "sdkconfig.h"

#include "MQTTClient.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_attr.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "fileTable.h"

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
 */

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

/* Constants that aren't configurable in menuconfig */
#define MQTT_SERVER "m15.cloudmqtt.com"
#define MQTT_USER "afhnaorh"
#define MQTT_PASS "HbzIMjkG3TkH"
#define MQTT_PORT 33521
#define MQTT_CLIENTID "GROUP2"
#define MQTT_WEBSOCKET 1  // 0=no 1=yes
#define MQTT_BUF_SIZE 512

xSemaphoreHandle print_mux;

static unsigned char mqtt_sendBuf[MQTT_BUF_SIZE];
static unsigned char mqtt_readBuf[MQTT_BUF_SIZE];

//static unsigned char mqtt_sendBuf[MQTT_BUF_SIZE];
//static unsigned char mqtt_readBuf[MQTT_BUF_SIZE];

static const char *CTAG = "MQTTS";





/* FreeRTOS event group to signal when we are connected & ready to make a request */
EventGroupHandle_t wifi_event_group;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            /* This is a workaround as ESP32 WiFi libs don't currently
               auto-reassociate. */
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

void mqtt_task(void *pvParameters)
{
	int ret;
	//define values used in vTaskDelayUntil() to make task periodic
		TickType_t xLastWakeTime;
		//portTICK_RATE_MS converts from ms to ticks, allowing me to say
		//"10000 milliseconds" and not care about my board's tick rate
		const TickType_t xFrequency = 10000*portTICK_RATE_MS;
	while(1){
			xLastWakeTime = xTaskGetTickCount();
			Network network;
			MQTTClient client;
			NetworkInit(&network);
			network.websocket = MQTT_WEBSOCKET;
			
			
			xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
			
			ret = NetworkConnect(&network, MQTT_SERVER, MQTT_PORT);
			
			MQTTClientInit(&client, &network,
            2000,            // command_timeout_ms
            mqtt_sendBuf,         //sendbuf,
            MQTT_BUF_SIZE, //sendbuf_size,
            mqtt_readBuf,         //readbuf,
            MQTT_BUF_SIZE  //readbuf_size
            );

		char buf[30];
		MQTTString clientId = MQTTString_initializer;
		sprintf(buf, MQTT_CLIENTID);
		clientId.cstring = buf;

		MQTTString username = MQTTString_initializer;
		username.cstring = MQTT_USER;	
		
		MQTTString password = MQTTString_initializer;
		password.cstring = MQTT_PASS;
		
		MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
		data.clientID          = clientId;
		data.willFlag          = 0;
		data.MQTTVersion       = 4; // 3 = 3.1 4 = 3.1.1
		data.keepAliveInterval = 5;
		data.cleansession      = 1;
		data.username          = username;
		data.password          = password;
		
		ret = MQTTConnect(&client, &data);
		if (ret != SUCCESS) {
        printf("\n\nError I think: %d\n\n",ret);
		goto exit;
		}
		printf("\n\nError I thinkn't: %d\n\n",ret);		
		
		
		char msgbuf[200];
		MQTTMessage message;
		message.qos = QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)msgbuf;
        message.payloadlen = strlen(msgbuf)+1;
		
		if(toComm != -1234){

	switch(toComm%10){
		case 0:
			//publish to iupui/group2/temperature
			printf("\n\nPublish to temp\n\n");
			sprintf(msgbuf, "{\"temperature\":%d,}", toComm);
			printf("\n\n%s\n\n",msgbuf);	
			ret = MQTTPublish(&client, "iupui/group2/", &message);
			toComm = -1234;
			if (ret != SUCCESS) {
					printf("MQTTPublish NOT: %d", ret);
			}else{
				printf("MQTTPublish wait I think it did: %d",ret);
			}	
		 
			break;
		case 1:
			//publish to iupui/group2/humidity
			printf("\n\nPublish to humidity\n\n");
			sprintf(msgbuf, "{\"humidity\":%d,}", toComm);
			printf("\n\n%s\n\n",msgbuf);	
			ret = MQTTPublish(&client, "iupui/group2/", &message);
			toComm = -1234;
			if (ret != SUCCESS) {
				printf("MQTTPublish NOT: %d", ret);
			}else{
				printf("MQTTPublish wait I think it did: %d",ret);
			}	
			
			break;
		case 2: 
			//publish to iupui/group2/light
			printf("\n\nPublish to light\n\n");
			
			sprintf(msgbuf, "{\"light\":%d,}", toComm);
			printf("\n\n%s\n\n",msgbuf);	
			ret = MQTTPublish(&client, "iupui/group2/", &message);
			toComm = -1234;
			if (ret != SUCCESS) {
				printf("MQTTPublish NOT: %d", ret);
			}else{
				printf("MQTTPublish wait I think it did: %d",ret);
			}	
			
			break;
		default:
			//no publish
			printf("\n\n | COMM TASK ERROR | Out-of-bounds value put into toComm\n\n");
			break;

			}//end switch case
		
			
			

		}
			
			printf("\n\n| COMM TASK | Before delay\n\n");
			vTaskDelayUntil(&xLastWakeTime, xFrequency);
			printf("\n\n| COMM TASK | After delay\n\n");
	
exit:
    MQTTDisconnect(&client);
    NetworkDisconnect(&network);
	}
}


static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "ThatGoodGood",
            .password = "iFhPtHm69420",
        },
    };
    ESP_LOGI(CTAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}


void comms_open()
{
    nvs_flash_init();
    initialise_wifi();

    // MQTT and TLS needs a deep stack, 12288 seems to work, anything
    // less will get us a stack overflow error
    xTaskCreate(&mqtt_task, "mqtt_task", 12288, NULL, 5, NULL);

}
