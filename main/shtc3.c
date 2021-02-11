#include "shtc3.h"

uint8_t calc_crc8(uint8_t *data, size_t len);
static const char *TAG = "SHCT";

void init_sensor(i2c_mode_t mode, gpio_num_t sda, gpio_num_t scl, uint32_t freq)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_NUM_21;
    conf.scl_io_num = GPIO_NUM_22;

    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;

    conf.master.clk_speed = freq;

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode,
                       0,
                       0,
                       0);
}

esp_err_t wakeup_sensor(uint8_t addr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x35, true);
    i2c_master_write_byte(cmd, 0x17, true);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 2);
    i2c_cmd_link_delete(cmd);
    if(err)
        ESP_LOGI(TAG, "%s", esp_err_to_name(err));
    return err;
}
esp_err_t get_ID_sensor(uint8_t addr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0xEF, true);
    i2c_master_write_byte(cmd, 0xC8, true);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 2);
    i2c_cmd_link_delete(cmd);

    if(err)
        ESP_LOGI(TAG, "%s", esp_err_to_name(err));

    return err;
}


esp_err_t soft_reset_sensor(uint8_t addr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x80, true);
    i2c_master_write_byte(cmd, 0x5D, true);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 2);
    i2c_cmd_link_delete(cmd);

    return err;
}

esp_err_t read_out(uint8_t addr, uint16_t read_mode, float *temp, float *hum)
{
    vTaskDelay(10 / portTICK_RATE_MS);
    //Read T First
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x78, true);
    i2c_master_write_byte(cmd, 0x66, true);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 2);
    if (err)
        return err;

    i2c_cmd_link_delete(cmd);

    vTaskDelay(30 / portTICK_RATE_MS);

    //Read
    uint8_t data[6];

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0xE1, true);

    i2c_master_read(cmd, data, 6, I2C_MASTER_LAST_NACK);

    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 2);
    i2c_cmd_link_delete(cmd);

    if (calc_crc8(data, 2) == data[2])
        *temp = ((((data[0] << 8) + data[1]) * 175) / 65536.0) - 45;
    else
        ESP_LOGI(TAG, "Temperature CRC error");

    
    if (calc_crc8(data + 3, 2) == data[5])
        *hum = ((((data[3] << 8) + data[4]) * 100) / 65536.0);
    else
        ESP_LOGI(TAG, "Humidity CRC error");

    
    return err;
}

esp_err_t sleep_sensor(uint8_t addr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0xB0, true);
    i2c_master_write_byte(cmd, 0x98, true);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 2);
    i2c_cmd_link_delete(cmd);

    return err;
}

uint8_t calc_crc8(uint8_t *data, size_t len)
{
    uint8_t crc = 0xff;
    size_t i, j;
    for (i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (j = 0; j < 8; j++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    return crc;
}