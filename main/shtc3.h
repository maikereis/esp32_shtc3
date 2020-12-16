#ifndef __SHTC3_H__
#define __SHTC3_H__

#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

//CS =  CLOCK_STRETCHING
//LP = LOW POWER
//N = NORMAL
#define T_FIRST_N_CS 0x7CA2
#define RH_FIRST_N_CS 0x5C24
#define T_FIRST_LP_CS 0x6458
#define RH_FIRST_LP_CS 0x44DE

#define T_FIRST_N 0x7866
#define RH_FIRST_N 0x58E0
#define T_FIRST_LP 0x609C
#define RH_FIRST_LP 0x401A

#define I2C_MASTER_FREQ_100KHZ 100000

void init_sensor(i2c_mode_t mode, gpio_num_t sda, gpio_num_t scl, uint32_t freq);
esp_err_t wakeup_sensor(uint8_t addr);
esp_err_t read_out(uint8_t addr, uint16_t read_mode, float *temp, float *hum);
esp_err_t sleep_sensor(uint8_t addr);




#endif