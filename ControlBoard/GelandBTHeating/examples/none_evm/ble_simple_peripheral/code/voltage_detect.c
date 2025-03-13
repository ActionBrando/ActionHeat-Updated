#include "voltage_detect.h"
#include "driver_adc.h"
#include "os_timer.h"
#include "temp_control.h"
#include "driver_pwm.h"

#define VOL_DETECT_INTERVAL 10000

uint8_t need_get_vol = 0;
uint32_t real_adc_val = 0;

uint32_t sub_real_adc_val = 0;

os_timer_t vol_detect_timer;
float detected_vol = 0;
uint8_t is_vol_ready = 0;

void (*upload_vol)(uint16_t);

void vol_detect_init(void (*upload_func)(uint16_t))
{
    os_timer_init(&vol_detect_timer, detect_vol_timer_handler, NULL);
    upload_vol = upload_func;
    vol_detect_start();
}

void vol_detect_start()
{
    os_timer_start(&vol_detect_timer, VOL_DETECT_INTERVAL, true);
}

void vol_detect_stop()
{
    os_timer_stop(&vol_detect_timer);
}

void detect_vol_timer_handler(void *param)
{
    need_get_vol = 1;
}

void process_vol_detect()
{
    if (need_get_vol == 0)
    {
        return;
    }
    need_get_vol = 0;

    // stop heating first
    // if (get_heating_status() == STATUS_ON)
    // {
    //     stop_ctr_heating();
    // }

    // co_delay_100us(10);
    // detect voltage
    real_adc_val = adc_get_channel_data(1);
    detected_vol = (float)real_adc_val / 1024 * 3.3 * 11.4;

    // sub_real_adc_val = adc_get_channel_data(7);
    // sub_detected_vol = (float)sub_real_adc_val / 1024 * 3.3;

    // co_printf("detected_vol:%d,sub_detected_vol:%d\r\n", (uint16_t)(detected_vol * 100), (uint16_t)(sub_detected_vol * 100));
    //  resume start heating
    // if (get_heating_status() == STATUS_ON)
    // {
    //     start_ctr_heating();
    // }

    if (upload_vol != NULL)
    {
        upload_vol(get_detected_vol());
    }
}

uint8_t get_main_vol_val(void)
{
    uint32_t temp_adc_val = adc_get_channel_data_FT(1);
    return (uint8_t)((float)temp_adc_val / 1024 * 3.3 * 11.4) * 10;
}

uint16_t get_detected_vol()
{
    return detected_vol * 100;
}
