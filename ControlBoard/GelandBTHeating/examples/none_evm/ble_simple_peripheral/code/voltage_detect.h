
#ifndef VOL_DETECT_H
#define VOL_DETECT_H

#include <stdint.h>

void vol_detect_init(void (*upload_func)(uint16_t));
void vol_detect_start(void);
void vol_detect_stop(void);
void process_vol_detect(void);
uint16_t get_detected_vol(void);
uint16_t get_sub_detected_vol(void);
void detect_vol_timer_handler(void *param);
void wait_vol_timer_handler(void *param);
uint8_t get_main_vol_val(void);

#endif
