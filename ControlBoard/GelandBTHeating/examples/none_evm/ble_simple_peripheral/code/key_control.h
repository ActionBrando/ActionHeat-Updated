
#ifndef KEY_CONTROL_H
#define KEY_CONTROL_H
#include <stdint.h>

/**
 * @brief process key short click event
 *
 * @return * void
 */
void process_key_event(void);
void power_key_timer_handler(void *param);
void led_key_timer_handler(void *param);
void key_control_init(void);

#endif
