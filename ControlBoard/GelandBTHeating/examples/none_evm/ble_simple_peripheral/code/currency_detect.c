#include "currency_detect.h"
#include "driver_adc.h"
#include "os_timer.h"
#include "temp_control.h"
#include "co_log.h"
#include "ble_simple_peripheral.h"

#define CUL_DETECT_INTERVAL 15 // 5
#define WAIT_DUTY_INTERVAL 2

#define ABNORMAL_MAX_COUNT 5

uint32_t real_cur_val = 0;
os_timer_t cur_detect_timer;
float detected_cur = 0;
float init_cur_vol = 0;

uint8_t need_get_cur = 0;

uint8_t abnormal_count = 0;

void cur_detect_init()
{
    get_init_cur_vol();
    os_timer_init(&cur_detect_timer, detect_cur_timer_handler, NULL);
    cur_detect_start();
}

void cur_detect_start()
{
    os_timer_start(&cur_detect_timer, CUL_DETECT_INTERVAL, false);
}

void cur_detect_stop()
{
    os_timer_stop(&cur_detect_timer);
}

void detect_cur_timer_handler(void *param)
{
    need_get_cur = 1;
}

void process_cur_detect()
{
    // uint8_t heating_duty;
    float cur;
    uint16_t i_cur;
    if (need_get_cur == 0)
    {
        return;
    }
    need_get_cur = 0;
    if (get_heating_status() == 0)
    {
        cur_detect_start();
        return;
    }
    // heating_duty = get_cur_pwm_duty();
    // if (heating_duty != 100)
    // {
    //     set_heating_pwm_duty_not_record(100);
    //     co_delay_10us(3);
    // }
    real_cur_val = adc_get_channel_data(0);
    // if (get_heating_status() != 0 && heating_duty != 100)
    // {
    //     heating_duty = get_cur_pwm_duty();
    //     set_heating_pwm_duty_not_record(heating_duty);
    // }

    detected_cur = (float)real_cur_val / 1024 * 3.3;

    // 电阻-0.025
    // cur = (detected_cur - init_cur_vol) / 20.6 / 0.025 * heating_duty / 100;
    cur = detected_cur * 92 / 52;

    //i_cur = (uint16_t)(cur * 1000);
    //co_printf("i_cur:%d,detected_cur:%d,real_cur_val:%d\r\n", i_cur, (uint16_t)(detected_cur * 1000), real_cur_val);
		if ((detected_cur > 2 && (DEVICE_TYPE != 3 && DEVICE_TYPE != 2)) || (detected_cur > 1.7 && (DEVICE_TYPE == 3 || DEVICE_TYPE == 2)) )
		{
				abnormal_shutdown();
				cur_detect_start();
				return;
		}
    if ((cur > 2.6 && (DEVICE_TYPE != 3 && DEVICE_TYPE != 2)) || (cur > 2 && (DEVICE_TYPE == 3 || DEVICE_TYPE == 2)))
    {
        abnormal_count++;
        if (abnormal_count >= ABNORMAL_MAX_COUNT)
        {
            abnormal_count = 0;
            abnormal_shutdown();
            cur_detect_start();
            return;
        }
    }
    else
    {
        abnormal_count = 0;
    }
    cur_detect_start();
}

void get_init_cur_vol()
{
    real_cur_val = adc_get_channel_data(0);
    init_cur_vol = (float)real_cur_val / 1024 * 3.3;
}

uint8_t get_detected_cur()
{
    return detected_cur * 10;
}
