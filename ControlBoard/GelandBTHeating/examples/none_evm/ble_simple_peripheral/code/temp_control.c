#include "temp_control.h"

#include "driver_gpio.h"
#include "driver_pmu.h"
#include "driver_adc.h"
#include "driver_pwm.h"
#include "led_control.h"
#include "os_timer.h"

#define LEVEL_STABLE_HEATING_INTERVAL 900000 // 15 minutes
#define LEVEL_CHANGE_HEATING_INTERVAL 15000	 // 15 seconds

/************************************************************************************/

uint8_t cur_heating_level = DEFAULT_LEVEL;
uint8_t cur_temp = 0;
uint8_t heating_status = STATUS_OFF;
uint8_t cur_pwm_level = 0;
uint8_t cur_pwm_duty = 0;

os_timer_t heating_level_timer;

void (*upload_temp)(uint8_t, uint8_t);

uint8_t level_heating_flag = 0; // 0-not heating 1-stable heating 2-changing heating high 3-changing heating low

/************************************************************************************
 * @fn      temp_control_init
 *
 * @brief   temp control init
 */
void temp_control_init(void (*upload_func)(uint8_t, uint8_t))
{
	upload_temp = upload_func;
	os_timer_init(&heating_level_timer, level_heating_timer_handler, NULL);
}

uint8_t get_heating_level(void)
{
	return cur_heating_level;
}

/************************************************************************************
 */
void start_ctr_heating(void)
{
	pwm_output_enable(PWM_CHANNEL_3);
}

/************************************************************************************
 */
void stop_ctr_heating(void)
{
	pwm_output_disable(PWM_CHANNEL_3);
}

/************************************************************************************
 * @fn      start_heating
 *
 * @brief   start heating
 */
void start_heating(void)
{
	if (heating_status == STATUS_ON)
	{
		return;
	}
	pwm_output_enable(PWM_CHANNEL_3);
	start_led_control();
	set_pwm_level(HIGH_LEVEL);
	heating_status = STATUS_ON;
	level_heating_flag = 1;
}

/************************************************************************************
 * @fn      stop_heating
 *
 * @brief   stop heating
 */
void stop_heating(void)
{
	if (heating_status == STATUS_OFF)
	{
		return;
	}
	heating_status = STATUS_OFF;
	set_heating_pwm_duty(0);
	pwm_output_disable(PWM_CHANNEL_3);
	stop_led_control();
}

/************************************************************************************
 */
uint8_t get_heating_status(void)
{
	return heating_status;
}

void set_heating_status(uint8_t status)
{
	if (status == heating_status)
	{
		return;
	}
	if (status == STATUS_ON)
	{
		start_heating();
	}
	else
	{
		stop_heating();
		os_timer_stop(&heating_level_timer);
	}
}

void level_heating_timer_handler(void *param)
{
	//  0-not heating 1-stable heating 2-changing heating high 3-changing heating low
	if (level_heating_flag == 1)
	{
		change_heating_pwm_triggle(1);
		level_heating_flag = 2;
		os_timer_start(&heating_level_timer, LEVEL_CHANGE_HEATING_INTERVAL, false);
	}
	else if (level_heating_flag == 2)
	{
		change_heating_pwm_triggle(0);
		level_heating_flag = 3;
		os_timer_start(&heating_level_timer, LEVEL_CHANGE_HEATING_INTERVAL, false);
	}
	else if (level_heating_flag == 3)
	{
		change_heating_pwm_triggle(1);
		level_heating_flag = 2;
		os_timer_start(&heating_level_timer, LEVEL_CHANGE_HEATING_INTERVAL, false);
	}
}

void change_heating_pwm_triggle(uint8_t is_high)
{
	switch (cur_heating_level)
	{
	case HIGH_LEVEL:
		if (is_high)
		{
			set_heating_pwm_duty(PWM_HIGH_DUTY);
		}
		else
		{
			set_heating_pwm_duty(PWM_HIGH_DUTY_SUB);
		}
		break;
	case MID_LEVEL:
		if (is_high)
		{
			set_heating_pwm_duty(PWM_MID_DUTY);
		}
		else
		{
			set_heating_pwm_duty(PWM_MID_DUTY_SUB);
		}
		break;
	case LOW_LEVEL:
		if (is_high)
		{
			set_heating_pwm_duty(PWM_LOW_DUTY);
		}
		else
		{
			set_heating_pwm_duty(PWM_LOW_DUTY_SUB);
		}
		break;
	default:
		break;
	}
}

/************************************************************************************
 * @fn      set_heating_level
 *
 * @brief   set heating temp by level
 */
void set_pwm_level(uint8_t level)
{

	// if (cur_heating_level == level || heating_status == STATUS_OFF)
	// {
	// 	return;
	// }
	cur_heating_level = level;
	if (level == LOW_LEVEL)
	{
		set_heating_pwm_duty(PWM_LOW_DUTY);
	}
	else if (level == MID_LEVEL)
	{
		set_heating_pwm_duty(PWM_MID_DUTY);
	}
	else
	{
		set_heating_pwm_duty(PWM_HIGH_DUTY);
	}
	os_timer_stop(&heating_level_timer);
	os_timer_start(&heating_level_timer, LEVEL_STABLE_HEATING_INTERVAL, false);
	set_led_level(cur_heating_level);
}

uint8_t get_cur_pwm_duty(void)
{
	return cur_pwm_duty;
}

void set_heating_pwm_duty(uint8_t duty)
{
	cur_pwm_duty = duty;
	struct_PWM_Config_t PWM_Config;
	PWM_Config.Prescale = 250000; // 2Hz, 50000 - 10Hz
	PWM_Config.Period = 100;
	PWM_Config.Posedge = 0;
	PWM_Config.Negedge = duty;
	pwm_config(PWM_CHANNEL_3, PWM_Config);
	pwm_output_updata(PWM_CHANNEL_3);
	pwm_output_enable(PWM_CHANNEL_3);
}

void set_heating_pwm_duty_not_record(uint8_t duty)
{
	struct_PWM_Config_t PWM_Config;
	PWM_Config.Prescale = 250000; // 2Hz, 50000 - 10Hz
	PWM_Config.Period = 100;
	PWM_Config.Posedge = 0;
	PWM_Config.Negedge = duty;
	pwm_config(PWM_CHANNEL_3, PWM_Config);
	pwm_output_updata(PWM_CHANNEL_3);
	pwm_output_enable(PWM_CHANNEL_3);
}

void init_output_as_gpio(void)
{
	GPIO_InitTypeDef GPIO_Handle;
	GPIO_Handle.Pin = GPIO_PIN_3;
	GPIO_Handle.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull = GPIO_PULLUP;
	gpio_init(GPIO_C, &GPIO_Handle);
}

void set_output_pin(uint8_t high)
{
	if (high)
	{
		gpio_write_pin(GPIO_C, GPIO_PIN_3, GPIO_PIN_SET);
	}
	else
	{
		gpio_write_pin(GPIO_C, GPIO_PIN_3, GPIO_PIN_CLEAR);
	}
}

void init_output_as_pwm(void)
{
	GPIO_InitTypeDef GPIO_Handle;
	GPIO_Handle.Pin = GPIO_PIN_3;
	GPIO_Handle.Mode = GPIO_MODE_AF_PP;
	GPIO_Handle.Pull = GPIO_PULLDOWN;
	GPIO_Handle.Alternate = GPIO_FUNCTION_6;
	gpio_init(GPIO_C, &GPIO_Handle);
}
