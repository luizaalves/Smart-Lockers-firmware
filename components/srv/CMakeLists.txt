idf_component_register(SRCS "src/srv_wifi.c" "src/srv_led.c" 
                    INCLUDE_DIRS "inc"
                    PRIV_REQUIRES drv)

target_include_directories(${COMPONENT_LIB} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IDF_PATH}/components/esp_wifi/include 
    ${IDF_PATH}/components/esp_event/include
    ${IDF_PATH}/components/esp_netif/include
    ${IDF_PATH}/components/nvs_flash/include
    ${IDF_PATH}/components/esp_partition/include
)