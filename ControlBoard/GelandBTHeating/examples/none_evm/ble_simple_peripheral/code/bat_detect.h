#ifndef _BAT_DETECT__H__
#define _BAT_DETECT__H__
#include <stdio.h>
#include <stdint.h>
#include "driver_timer.h"

#define BAT_TIMER TIMER1

// 48000000 prescale 256 187.5KHz
#define START_CODE_LOW_COUNT_MAX 250  // 5.2ms = (0.0052/(1/40000))*1.2 = 250
#define START_CODE_LOW_COUNT_MIN 166  // 5.2ms = (0.0052/(1/40000))*0.8 = 166
#define START_CODE_HIGH_COUNT_MAX 134 // 2.8ms = (0.0028/(1/40000))*1.2 = 134
#define START_CODE_HIGH_COUNT_MIN 90  // 2.8ms = (0.0028/(1/40000))*0.8 = 90
#define DATA_CODE_LONG_COUNT_MAX 58   // 1.2ms = (0.0012/(1/40000))*1.2 = 58
#define DATA_CODE_LONG_COUNT_MIN 38   // 1.2ms = (0.0012/(1/40000))*0.8 = 38
#define DATA_CODE_SHORT_COUNT_MAX 19  // 0.4ms = (0.0004/(1/40000))*1.2 =  19
#define DATA_CODE_SHORT_COUNT_MIN 8   // 0.4ms = (0.0004/(1/40000))*0.8 =  13

void process_bat_event(void);
void bat_detect_init(void (*p_callback)(uint8_t));
uint8_t check_level_valid(uint8_t level, uint32_t start, uint32_t end);
uint8_t decode_data_bit(void);
uint32_t get_timer_count(void);
void reset_state(void);
uint8_t get_bat_num(void);
void bat_detect_start(void);
void bat_detect_stop(void);
#endif
