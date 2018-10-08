/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <sodium.h>
#include <string.h>

#define PASSWORD "password"

void app_main()
{
	// Bree: This is for troubleshooting / error-handling for libsodium (i.e. argon2)
	if (sodium_init() < 0)
	{
	printf("Sodium failed!\n");
	return;
	}

	char hashed_password[crypto_pwhash_STRBYTES];
	unsigned char salt[crypto_pwhash_SALTBYTES];
	unsigned char key[crypto_box_SEEDBYTES];
	randombytes_buf(salt, sizeof salt);
	
	// Bree: All print statements for error handling tests for argon2 functionality....
	// Bree: Remember!!!!
	//       In menuconfig: set MAIN TASK SIZE >= 8192 and set XTAL FREQ = 26 MHz
	
	if (crypto_pwhash_str(hashed_password, PASSWORD, strlen(PASSWORD), 3, 3) == -1) {
	printf("FAILED1!\n");
	}
	if (crypto_pwhash_str(hashed_password, PASSWORD, strlen(PASSWORD), 3, 10) == -1) {
	printf("FAILED2!\n");
	}
	if (crypto_pwhash_str(hashed_password, PASSWORD, strlen(PASSWORD), 3, 50) == -1) {
	printf("FAILED3!\n");
	}
	if (crypto_pwhash_str(hashed_password, PASSWORD, strlen(PASSWORD), 3, 150) == -1) {
	printf("FAILED4!\n");
	}
	if (crypto_pwhash_str(hashed_password, PASSWORD, strlen(PASSWORD), crypto_pwhash_OPSLIMIT_SENSITIVE, 512) == -1) {
	printf("FAILED5!\n");
	}
	if (crypto_pwhash_str(hashed_password, PASSWORD, strlen(PASSWORD), 3, 1024) == -1) {
	printf("FAILED6!\n");
	}
	if (crypto_pwhash_str(hashed_password, PASSWORD, strlen(PASSWORD), 3, 4096) == -1) {
	printf("FAILED7!\n");
	}
	if (crypto_pwhash_str(hashed_password, PASSWORD, strlen(PASSWORD), 3, 500000) == -1) {
	printf("FAILED8!\n");
	}
	if (crypto_pwhash_str(hashed_password, PASSWORD, strlen(PASSWORD), 3, crypto_pwhash_MEMLIMIT_INTERACTIVE) == -1) {
	printf("FAILED9!\n");
	}
	if (crypto_pwhash(key, sizeof key, PASSWORD, strlen(PASSWORD), salt, 3, 100, crypto_pwhash_ALG_DEFAULT) != 0)
	{
	printf("FAILED10!\n");
	}
	if (crypto_pwhash(key, sizeof key, PASSWORD, strlen(PASSWORD), salt, crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0)
	{
	printf("FAILED11!\n");
	}
//return;

// Bree: Here is the basic Hello World program (no more libsodium / argon2 error finding)
    printf("Hello world!\n");
    printf("\nWe are:\nBree\nDamon\nKouma\nSohin\n\n");
	
	
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("\nsilicon revision %d, ", chip_info.revision);

    printf("\n%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (int i = 10; i >= 0; i--) {
        printf("\nRestarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
