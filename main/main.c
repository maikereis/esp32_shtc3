#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#define I2C_MASTER_FREQ_HZ 100000

const char *TAG = "MAIN";

void app_main(void)
{
    //config i2c

    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_NUM_21;
    conf.scl_io_num = GPIO_NUM_22;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode,
                       0,
                       0,
                       0);

    //Wake-up
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0xE0, true);
    i2c_master_write_byte(cmd, 0x35, true);
    i2c_master_write_byte(cmd, 0x17, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 2);
    i2c_cmd_link_delete(cmd);

    //Read T First
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0xE0, true);
    i2c_master_write_byte(cmd, 0x78, true);
    i2c_master_write_byte(cmd, 0x66, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 2);
    i2c_cmd_link_delete(cmd);

    vTaskDelay(30 / portTICK_RATE_MS);

    //Read
    uint8_t data[6];

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0xE1, true);

    i2c_master_read(cmd, data, 6, I2C_MASTER_LAST_NACK);

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 2);
    i2c_cmd_link_delete(cmd);

    float T = 0, H = 0;
    T = ((((data[0] << 8) + data[1]) * 175) / 65536.0) - 45;

    H = ((((data[3] << 8) + data[4]) * 100) / 65536.0);
    ESP_LOGI(TAG, "Temperature: %f, Humidade: %f", T, H);
}
