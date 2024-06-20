idf_component_register(SRCS "src/srv_wifi.c" "src/srv_mqtt.c" "src/srv_led.c" "src/srv_communication.c" "src/srv_rfid.c"
                    INCLUDE_DIRS "inc"
                    PRIV_REQUIRES drv rc522)

target_include_directories(${COMPONENT_LIB} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IDF_PATH}/components/esp_wifi/include 
    ${IDF_PATH}/components/esp_event/include
    ${IDF_PATH}/components/esp_netif/include
    ${IDF_PATH}/components/nvs_flash/include
    ${IDF_PATH}/components/esp_partition/include
    ${IDF_PATH}/components/mqtt/esp-mqtt/include
    ${IDF_PATH}/components/tcp_transport/include
    ../rc522/include
)