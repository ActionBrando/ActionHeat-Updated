
#ifndef CUR_DETECT_H
#define CUR_DETECT_H

#include <stdint.h>

void cur_detect_init(void);
void cur_detect_start(void);
void cur_detect_stop(void);
void process_cur_detect(void);
uint8_t get_detected_cur(void);
void detect_cur_timer_handler(void *param);
void get_init_cur_vol(void);

#endif
