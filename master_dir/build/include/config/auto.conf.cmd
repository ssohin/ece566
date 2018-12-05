deps_config := \
	/home/baabe/esp-idf/components/app_trace/Kconfig \
	/home/baabe/esp-idf/components/aws_iot/Kconfig \
	/home/baabe/esp-idf/components/bt/Kconfig \
	/home/baabe/esp-idf/components/driver/Kconfig \
	/home/baabe/esp-idf/components/esp32/Kconfig \
	/home/baabe/esp-idf/components/esp_adc_cal/Kconfig \
	/home/baabe/esp-idf/components/esp_http_client/Kconfig \
	/home/baabe/esp-idf/components/ethernet/Kconfig \
	/home/baabe/esp-idf/components/fatfs/Kconfig \
	/home/baabe/esp-idf/components/freertos/Kconfig \
	/home/baabe/esp-idf/components/heap/Kconfig \
	/home/baabe/esp-idf/components/http_server/Kconfig \
	/home/baabe/esp-idf/components/libsodium/Kconfig \
	/home/baabe/esp-idf/components/log/Kconfig \
	/home/baabe/esp-idf/components/lwip/Kconfig \
	/home/baabe/esp-idf/components/mbedtls/Kconfig \
	/home/baabe/esp-idf/components/mdns/Kconfig \
	/home/baabe/esp-idf/components/openssl/Kconfig \
	/home/baabe/esp-idf/components/pthread/Kconfig \
	/home/baabe/esp-idf/components/spi_flash/Kconfig \
	/home/baabe/esp-idf/components/spiffs/Kconfig \
	/home/baabe/esp-idf/components/tcpip_adapter/Kconfig \
	/home/baabe/esp-idf/components/vfs/Kconfig \
	/home/baabe/esp-idf/components/wear_levelling/Kconfig \
	/home/baabe/esp-idf/Kconfig.compiler \
	/home/baabe/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/baabe/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/baabe/esp-idf/examples/get-started/ece566/master_dir/main/Kconfig.projbuild \
	/home/baabe/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/baabe/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
