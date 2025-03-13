
#ifndef TEMP_CONTROL_H
#define TEMP_CONTROL_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

// #define OFF_LEVEL 0
#define LOW_LEVEL 0
#define MID_LEVEL 1
#define HIGH_LEVEL 2
#define DEFAULT_LEVEL LOW_LEVEL

#define PWM_HIGH_DUTY 100
#define PWM_HIGH_DUTY_SUB 80

#define PWM_MID_DUTY 75
#define PWM_MID_DUTY_SUB 50

#define PWM_LOW_DUTY 50
#define PWM_LOW_DUTY_SUB 30

#define STATUS_ON 1
#define STATUS_OFF 0
#define STATUS_DEFAULT STATUS_ON

void temp_control_init(void (*upload_func)(uint8_t, uint8_t));
uint8_t get_heating_level(void);
void start_heating(void);
void start_ctr_heating(void);
void stop_heating(void);
void stop_ctr_heating(void);
uint8_t get_heating_status(void);
void set_heating_status(uint8_t status);
void set_heating_pwm_duty(uint8_t duty);
void set_pwm_level(uint8_t level);
void level_heating_timer_handler(void *param);
void change_heating_pwm_triggle(uint8_t is_high);
uint8_t get_cur_pwm_duty(void);
void init_output_as_gpio(void);
void set_output_pin(uint8_t high);
void init_output_as_pwm(void);
void set_heating_pwm_duty_not_record(uint8_t duty);
#endif
