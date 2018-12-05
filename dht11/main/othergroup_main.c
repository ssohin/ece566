
// 566 Project2 Group1
// Main.c ("Code sSection")

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sodium.h> 
#include <sys/time.h> 
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h" 
#include "esp_system.h" 
#include "nvs_flash.h" 
#include "nvs.h" 
#include "driver/gpio.h" 
#include "dht11.h" 
#include "sdkconfig.h" 
#include "driver/gpio.h" 
#include "rom/ets_sys.h" 
#include "soc/timer_group_struct.h"  
#include "soc/timer_group_reg.h" 
#include "esp_event_loop.h" 
#include "freertos/event_groups.h" 
#include "esp_wifi.h" 
#include "esp_log.h" 
#include "esp_attr.h" 
#include "esp_sleep.h" 
#include "lwip/err.h" 
#include "lwip/apps/sntp.h" 

 

#define EXAMPLE_WIFI_SSID "ssid goes here" 
#define EXAMPLE_WIFI_PASS "password goes here" 

 

 

//Function protoypes 
void getstring(char *string, int length); 
int nvsget(nvs_handle my_handle, char *key, char value[50], esp_err_t err); 
int nvsset(nvs_handle my_handle, char *key, char value[50], esp_err_t err); 
int argparse(char *input, char *key, char *value); 
static void obtain_time(void); 
static void initialize_sntp(void); 
static void initialise_wifi(void); 
static esp_err_t event_handler(void *ctx, system_event_t *event); 

 

//Wifi stuff 
static EventGroupHandle_t wifi_event_group; 
const int CONNECTED_BIT = BIT0; 
static const char *TAG = "example"; 
RTC_DATA_ATTR static int boot_count = 0; 
static void obtain_time(void); 
static void initialize_sntp(void); 
static void initialise_wifi(void); 
static esp_err_t event_handler(void *ctx, system_event_t *event); 

 

//This is a periodic task that prints the time, date, temperature and humidity  
void task2(void *pvParameter) 
{ 
while(1) 
{ 
/*//Wifi Stuff 
++boot_count; 
time_t now; 
struct tm timeinfo; 
time(&now); 
localtime_r(&now, &timeinfo); 
 
// Is time set? If not, tm_year will be (1970 - 1900). 
if (timeinfo.tm_year < (2016 - 1900)) { 
obtain_time(); 
// update 'now' variable with current time 
time(&now); 
} 
char strftime_buf[64]; 

 

// Set timezone to Eastern Standard Time and print local time 
setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1); 
tzset(); 
localtime_r(&now, &timeinfo); 
strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo); 
ESP_LOGI(TAG, "The current date/time in Indiana is: %s", strftime_buf);*/ 

 

//DHT11 temperature sensing stuff 
setDHTPin(16); 
printf("Temperature reading %lf\n",getTemp()*1.8 + 32); //Print Temperature in F 
vTaskDelay(5000 / portTICK_RATE_MS); 
printf("Humidity reading %d\n",getHumidity()); //Print Humidity 

 

//Make the task block for 10 seconds 
vTaskDelay(10000 / portTICK_RATE_MS); 

} 

} 

 

void app_main() 
{ 
  char str_p[20]; //input str for password hashing 
  char keys[50][50]; //This array stores the names of keys created by the set command 
  char value[50], key[50]; //Strings to store a value or key  
  char shellstring[50]; //input string for shell commands 
  char hash_pass[crypto_pwhash_STRBYTES]; 
  //int attempts = 0; //implement multiple tries later 
  int foundflag = 0;  //stay 0 if not found 
  int shellstate = 1; //1 for logged on , 0 for logged off 
  int i, x = 0; //i = iterator, x = index for keys array 
  int error; 
  nvs_handle my_handle; 
  TaskHandle_t xHandle = NULL; 
 
  gpio_set_level(15, 0); 

 

  //Initialize sodium 
  if(sodium_init() < 0) 
  { 
    printf("Sodium could not be loaded"); 
    return; 
  } 

 

  // Initialize NVS 
  esp_err_t err = nvs_flash_init(); 
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) 
  { 
      // NVS partition was truncated and needs to be erased 
      // Retry nvs_flash_init 
      ESP_ERROR_CHECK(nvs_flash_erase()); 
      err = nvs_flash_init(); 
  } 
  ESP_ERROR_CHECK( err ); 

 

//Store ssid, password and system id in storage 
error = nvsset(my_handle, "ssid", EXAMPLE_WIFI_SSID, err); 
if (error == 1) //Close program if error 
return; 
error = nvsset(my_handle, "wifipassword", EXAMPLE_WIFI_PASS, err); 
if (error == 1) //Close program if error 
return; 
error = nvsset(my_handle, "sysID", "001", err); 
if (error == 1) //Close program if error 
return; 

 

//Store keys in keys array 
strcpy(keys[x], "ssid"); x++; 
strcpy(keys[x], "wifipassword"); x++; 
strcpy(keys[x], "sysID"); x++; 

 
  //Open NVS handle 
   err = nvs_open("storage", NVS_READWRITE, &my_handle); 
  if (err != ESP_OK) 
  { 
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err)); 
 return; 
  } 
  else 
  { 
    size_t size = crypto_pwhash_STRBYTES; 
    //Read the nvs handle for "password" and place it into hash_pass 
    err = nvs_get_str(my_handle, "password", &hash_pass, &size); 
    switch (err) 
    { 
            case ESP_OK:  //found password variable 
                printf("Please enter the password\n"); 
                foundflag = 1; 
                break;  //pass 

 

            case ESP_ERR_NVS_NOT_FOUND: //variable password needs to be created 
                printf("\nPlease create a password: \n"); 
                getstring(str_p, sizeof(str_p)); 

 

                //Create new password hash value 
                if (crypto_pwhash_str(hash_pass, str_p, strlen(str_p), 
crypto_pwhash_OPSLIMIT_INTERACTIVE, 8192) != 0) 
                { 
                  printf("Out of memory"); 
                return; 
                } 
                else  //Store hash value under "password" key 
                { 
                  err = nvs_set_str(my_handle, "password", hash_pass); 
                  if (err != ESP_OK) //Failed 
                  { 
                    printf("\nFailed to store new password in memory!\n"); 
                    return; 
                  } 
                  else  //All is well, commit the changes 
                  { 
                    err = nvs_commit(my_handle); 
                    if (err != ESP_OK)  //Failed 
                    { 
                      printf("\nFailed to commit new password storage!\n"); 
                      return; 
                    } 
                    else //All is still well, done 
                      break; 
					                    } 
                } 

 

            default : 
                printf("Error (%s) reading!\n", esp_err_to_name(err)); 
                return; 
    } 

 

    // Close handle 
    nvs_close(my_handle); 
  } 

 

  if (foundflag == 1)  //found the key in system 
  { 
    //asked for user to input password above 
getstring(str_p, sizeof(str_p)); 
    if (crypto_pwhash_str_verify(hash_pass, str_p, strlen(str_p)) != 0
    { 
      printf("\nWrong password!\n"); 
      return; /* wrong password */ 
    } 
  } 
 
  //Create task for periodic data reset 
  xTaskCreate(&task2, "task2", 4096, NULL, 5, &xHandle); 
 
  //This will be the loop for the shell (for commands log and set) 
  while(1) 
  { 
//Logged on state 
if (shellstate == 1) 
{ 
//Print prompt char amd take user command 
printf("\n$ "); 
getstring(shellstring, sizeof(shellstring)); 

 

//If user enter "log off" 
if (!strcmp(shellstring, "log off")) 
{ 
//Delete the task to suppress data output 
vTaskDelete( xHandle ); 
shellstate = 0;  
continue; 
} 

 

//If the user types "set" 
else if (!strcmp(shellstring, "set")) 
{ printf("\n"); 

 

//Iterate through keys and print key = value for each  
i = 0; 
while(i < x) //Go until NULL (first empty element) 
{ 
//Retrieve value from stroage 
error = nvsget(my_handle, keys[i], value, err); 

 

if (error == 1) //Close program if error 
return; 

 

printf("%s = %s\n", keys[i], value); //Print name = value pair 
i++; 

} 

} 

 

//Otherwise "set name=value" or invalid command 
else //Need to analyze input 
{ 
error = argparse(shellstring, key, value); 
if (error == 1) 
printf("\nInvalid Command\n"); 
else 
{ 
//After parsing,set store in NV storage 
error = nvsset(my_handle, key, value, err); 
if (error == 1) //Close program if error 
return; 

 

strcpy(keys[x], key); //Add to key array 
x++; 

} 

 

} 

 

} 

 

//Logged off state (Suppress periodic data printing) 
else 
{ 
//Only command possible is "log on" I think 
printf("\n"); 
getstring(shellstring, sizeof(shellstring)); 

 

if (!strcmp(shellstring, "log on")) 
{ 
//Create task for periodic data reset
xTaskCreate(&task2, "task2", 4096, NULL, 5, &xHandle); 
shellstate = 1;  
continue; 

} 

} 

 

  } 

 

  return; 
} 

 

/////////////////////////////////   Function definitions   /////////////////////////////////

 

 

//Getstring definition 
void getstring(char *string, int length) 
{ 
char ch; 
int i; 

 

//Gets-like function using getchar 
i = 0; 
while(i<length-1) //-1 is to make room for the null terminator \0 
{ 
ch = getchar(); 

 

//Only take valid characters as input 
if (ch >= 0 && ch <= 127) 
{ 
//If the character was the enter key, break 
if (ch == 10) 
break; 

 

//Otherwise add to the string and keep looking 
else 
{ 
string[i] = ch; 
i++; 
} 

} 

 

//This stops the watchdog timer from closing the program  
TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE; 
TIMERG0.wdt_feed=1; 
TIMERG0.wdt_wprotect=0; 

} 

 

//Append the null terminator at the end
string[i] = '\0'; 

 

return; 

} 

 

//Function for getting a value from NV stroage given the key 
int nvsget(nvs_handle my_handle, char *key, char value[50], esp_err_t err) 
{ 

 

//Open NVS handle 
  err = nvs_open("storage", NVS_READWRITE, &my_handle); 
  if (err != ESP_OK) 
  { 
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err)); 
 return 1; 
  } 
  else 
  { 
    size_t size = 128; //I arbitrarily chose this size 
    //Read the nvs handle for given key and place the value in the value variable 
    err = nvs_get_str(my_handle, key, value, &size); 
    switch (err) 
    { 
            case ESP_OK:  //Found the variable successfully  
                break;  //pass 

 

            case ESP_ERR_NVS_NOT_FOUND: //Not found/initialized 
                printf("\nNVS key does not exist!\n"); 
                return 1; //1 for error 

 

            default : 
                printf("Error (%s) reading!\n", esp_err_to_name(err)); 
                return 1; //1 for error 
    } 

 

    // Close handle 
    nvs_close(my_handle); 
  } 
 
  return 0; //0 for no error 
} 

 

//Function for storing value in NV storage under given key 
int nvsset(nvs_handle my_handle, char *key, char *value, esp_err_t err) 
{ 
//Open NVS handle 
  err = nvs_open("storage", NVS_READWRITE, &my_handle); 
  if (err != ESP_OK) 
    { 
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err)); 
 return 1; //1 for error 
  } 
  else 
  { 
//Store value under given key  
err = nvs_set_str(my_handle, key, value); 
if (err != ESP_OK) //Failed 
{ 
printf("\nFailed to store new value in memory!\n"); 
return 1; //1 for error 
} 
else //All is well, commit the changes 
{ 
err = nvs_commit(my_handle); 
if (err != ESP_OK)  //Failed 
{ 
printf("\nFailed to commit new password storage!\n"); 
return 1; //1 for error 
} 
} 
 
    // Close handle 
    nvs_close(my_handle); 
  } 
 
  return 0; //0 for no error 
} 

 

//Function to parse a "set name=value" string and place the name and val in strings 
int argparse(char *input, char *key, char *value) 
{ 
char *token, *temp; 

 

//First parse the string by spaces 
token = strtok(input, " "); 

 

if(strcmp(token, "set") != 0) //Check first arg is set 
return 1; //1 for error 

 

else 
{ 

token = strtok(NULL, "="); 

 

if(token == NULL) 
return 1; //1 for error 
else 
{ 

strcpy(key, token); //Save the key in key 

 

token = strtok(NULL, "=");  
if(token == NULL) 
return 1; //1 for error 

 

strcpy(value, token); //Save the value in value 

 

return 0; //0 for no error 

} 

 

} 

} 

/////////////////////////////////   Wifi/SNTP Func Defs   ///////////////////////////////// 

static void obtain_time(void) 
{ 
    ESP_ERROR_CHECK( nvs_flash_init() ); 
    initialise_wifi(); 
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, 
                        false, true, portMAX_DELAY); 
    initialize_sntp(); 

 

    // wait for time to be set 
    time_t now = 0; 
    struct tm timeinfo = { 0 }; 
    int retry = 0; 
    const int retry_count = 10; 
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) { 
        vTaskDelay(2000 / portTICK_PERIOD_MS); 
        time(&now); 
        localtime_r(&now, &timeinfo); 
    } 

 

    ESP_ERROR_CHECK( esp_wifi_stop() ); 
} 

 

static void initialize_sntp(void) 
{ 
    sntp_setoperatingmode(SNTP_OPMODE_POLL); 
    sntp_setservername(0, "pool.ntp.org"); 
    sntp_init(); 
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
            .ssid = EXAMPLE_WIFI_SSID, 
            .password = EXAMPLE_WIFI_PASS, 
        }, 
    }; 
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) ); 
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) ); 
    ESP_ERROR_CHECK( esp_wifi_start() ); 
} 

 

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
  
        esp_wifi_connect(); 
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT); 
        break; 
    default: 
        break; 
    } 
    return ESP_OK; 
} 