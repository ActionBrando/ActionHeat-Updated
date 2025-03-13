#include "key_control.h"

#include "driver_gpio.h"
#include "driver_pmu.h"
#include "ble_simple_peripheral.h"
#include "os_timer.h"

uint8_t power_key_press;
uint8_t power_key_long_press;
uint8_t led_key_press;

uint8_t bat_event_flag = 0;

os_timer_t power_key_timer;
os_timer_t led_key_timer;

uint32_t power_ms_count;
uint32_t led_ms_count;

void key_control_init(void)
{
  os_timer_init(&power_key_timer, power_key_timer_handler, NULL);
  os_timer_init(&led_key_timer, led_key_timer_handler, NULL);
}

void power_key_timer_handler(void *param)
{
  if (gpio_read_pin(GPIO_C, GPIO_PIN_2) == 0)
  {
    power_ms_count++;
    if (power_ms_count > 150)
    {
      power_key_long_press = 1;
      power_ms_count = 0;
      os_timer_stop(&power_key_timer);
    }
  }
  else
  {
    if (power_ms_count > 5)
    {
      power_key_press = 1;
    }
    power_ms_count = 0;
    os_timer_stop(&power_key_timer);
  }
}

void led_key_timer_handler(void *param)
{
  if (gpio_read_pin(GPIO_D, GPIO_PIN_1) == 0)
  {
    led_ms_count++;
  }
  else
  {
    if (led_ms_count > 5)
    {
      led_key_press = 1;
    }
    led_ms_count = 0;
    os_timer_stop(&led_key_timer);
  }
}

/************************************************************************************
 * @fn      exti_isr
 *
 * @brief   exti interrupt handler
 */
void exti_isr(void)
{
  if (exti_get_LineStatus(EXTI_LINE18_PC2))
  {
    exti_clear_LineStatus(EXTI_LINE18_PC2);
    os_timer_stop(&power_key_timer);
    os_timer_start(&power_key_timer, 10, true);
  }
  if (exti_get_LineStatus(EXTI_LINE25_PD1))
  {
    exti_clear_LineStatus(EXTI_LINE25_PD1);
    os_timer_stop(&led_key_timer);
    os_timer_start(&led_key_timer, 10, true);
  }
}

void process_key_event()
{
  if (power_key_press == 1)
  {
    power_key_press = 0;
    // change heating level
    triggle_power_level();
  }
  if (led_key_press == 1)
  {
    led_key_press = 0;
    // change led mode
    triggle_led_mode();
  }
  if (power_key_long_press == 1)
  {
    power_key_long_press = 0;
    // change heating status
    triggle_heating_status();
  }
}
