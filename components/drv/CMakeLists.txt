idf_component_register(SRCS "src/drv_wifi.c" "src/drv_mqtt.c" "src/drv_led.c"
                    INCLUDE_DIRS "inc"
                    REQUIRES mqtt esp_wifi)

target_include_directories(${COMPONENT_LIB} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IDF_PATH}/components/esp_wifi/include 
    ${IDF_PATH}/components/esp_event/include
    ${IDF_PATH}/components/esp_netif/include
    ${IDF_PATH}/components/nvs_flash/include
    ${IDF_PATH}/components/esp_partition/include
    ${IDF_PATH}/components/mqtt/esp-mqtt/include
    ${IDF_PATH}/components/tcp_transport/include
)