
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdio.h>
#include <string.h>
#include "driver_i2c.h"

#define LED_R_ADDR 0xB1
#define LED_W_ADDR 0xB0

#define LED_STATUS_OFF 0x00
#define LED_STATUS_ON 0x01

#define LED_COLOR_RED 0x01
#define LED_COLOR_WHITE 0x02
#define LED_COLOR_BLUE 0x03

void led_control_init(I2C_HandleTypeDef *i2c_handle);
void start_led_control(void);
void stop_led_control(void);
void set_led_mode(uint8_t led_mode);
uint8_t get_led_mode(void);
void set_led_level(uint8_t level);
void set_led_color(uint8_t color);

#endif
