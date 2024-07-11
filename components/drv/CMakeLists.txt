idf_component_register(SRCS "src/drv_wifi.c" "src/drv_mqtt.c" "src/drv_led.c" "src/drv_gpio.c" "src/drv_nvs.c"
                            "src/drv_webserver.c"
                    INCLUDE_DIRS "inc"
                    REQUIRES mqtt esp_wifi nvs_flash esp_http_server)

target_include_directories(${COMPONENT_LIB} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IDF_PATH}/components/esp_wifi/include 
    ${IDF_PATH}/components/esp_event/include
    ${IDF_PATH}/components/esp_netif/include
    ${IDF_PATH}/components/nvs_flash/include
    ${IDF_PATH}/components/esp_partition/include
    ${IDF_PATH}/components/mqtt/esp-mqtt/include
    ${IDF_PATH}/components/tcp_transport/include
    ${IDF_PATH}/components/driver/gpio/include
    ${IDF_PATH}/components/driver/esp_http_server/include
)