#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "shtc3.h"

#define SENSOR_ADDR 0x70

const char *TAG = "MAIN";

void app_main(void)
{
    init_sensor(I2C_MODE_MASTER, 21, 22, I2C_MASTER_FREQ_100KHZ);
    wakeup_sensor(SENSOR_ADDR);
    float temperature, humidity;
    read_out(SENSOR_ADDR, T_FIRST_N, &temperature, &humidity);
    sleep_sensor(SENSOR_ADDR);
    ESP_LOGI(TAG, "Temperature: %f, Humidade: %f", temperature, humidity);
}
