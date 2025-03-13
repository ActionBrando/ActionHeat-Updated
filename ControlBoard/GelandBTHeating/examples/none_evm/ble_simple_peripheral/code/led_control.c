#include "led_control.h"

#include "driver_gpio.h"
#include "driver_pmu.h"
#include "ble_stack.h"
#include "app_config.h"
#include "sys_utils.h"

/************************************************************************************/

I2C_HandleTypeDef *Led_I2C1_Handle;
GPIO_InitTypeDef GPIO_Handle;
uint8_t led_level = LED_LEVEL_DEFAULT;
uint8_t led_status;

/************************************************************************************
 * @fn      led_control_init
 *
 * @brief   led control init
 */
void led_control_init(I2C_HandleTypeDef *i2c_handle)
{
  uint8_t write_value = 0;
  Led_I2C1_Handle = i2c_handle;

  // // init SHEN pin-old version
  // GPIO_Handle.Pin = GPIO_PIN_0;
  // GPIO_Handle.Mode = GPIO_MODE_OUTPUT_PP;
  // GPIO_Handle.Pull = GPIO_PULLUP;
  // gpio_init(GPIO_A, &GPIO_Handle);
  // // set SHEN HIGH
  // gpio_write_pin(GPIO_A, GPIO_PIN_0, GPIO_PIN_SET);

  // init SHEN pin-last version
  GPIO_Handle.Pin = GPIO_PIN_7;
  GPIO_Handle.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Handle.Pull = GPIO_PULLUP;
  gpio_init(GPIO_C, &GPIO_Handle);
  // set SHEN HIGH
  gpio_write_pin(GPIO_C, GPIO_PIN_7, GPIO_PIN_SET);

  co_delay_100us(60);
  write_value = 0x7F;
  // i2c_master_transmit(Led_I2C1_Handle,LED_W_ADDR,&write_value,1);
  i2c_memory_read(Led_I2C1_Handle, LED_R_ADDR, 0x7F, &write_value, 1);
  // init OUT6-OUT9 GPIO OUT0-OUT5 LED
  write_value = 0x3C;
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x12, &write_value, 1);
  write_value = 0x00;
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x13, &write_value, 1);
  // OUT6-OUT9 OUTPUT
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x04, &write_value, 1);
  // OUT6-OUT9 OUTPUT PUSH-PULL
  write_value = 0x3E;
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x11, &write_value, 1);
  // OUT6-OUT9 OUTPUT LOW
  write_value = 0x00;
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x02, &write_value, 1);

  // OUT0-OUT5 shutdown breath
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x14, &write_value, 1);
  // OUT0-OUT5 set color 0x00
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x20, &write_value, 1);
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x21, &write_value, 1);
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x22, &write_value, 1);
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x23, &write_value, 1);
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x24, &write_value, 1);
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x25, &write_value, 1);
  led_status = LED_STATUS_OFF;
}

void start_led_control()
{
  led_status = LED_STATUS_ON;
  set_led_level(led_level);
}

void stop_led_control()
{
  // OUT6-OUT9 OUTPUT LOW
  uint8_t write_value = 0x00;
  i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x02, &write_value, 1);
  led_status = LED_STATUS_OFF;
}

void set_led_mode(uint8_t led_mode)
{
  if (led_mode == led_status)
  {
    return;
  }
  if (led_mode == LED_STATUS_OFF)
  {
    stop_led_control();
  }
  else
  {
    start_led_control();
  }
}

uint8_t get_led_mode()
{
  return led_status;
}

/**
 * @brief Set the led level
 *
 * @param level
 */
void set_led_level(uint8_t level)
{
  led_level = level;
  if (led_status == LED_STATUS_OFF)
  {
    return;
  }
  /*
  if (level == led_level)
  {
    return;
  }
  */
  if (level == LED_LEVEL_LOW)
  {
    // set led blue
    set_led_color(LED_COLOR_BLUE);
  }
  else if (level == LED_LEVEL_MEDIUM)
  {
    // set led white
    set_led_color(LED_COLOR_WHITE);
  }
  else if (level == LED_LEVEL_HIGH)
  {
    // set led red
    set_led_color(LED_COLOR_RED);
  }
}

/**
 * @brief Set the led color
 *
 * @param color
 *
 * @remark high red, medium white, low blue
 */
void set_led_color(uint8_t color)
{
  uint8_t write_value;
  if (color == LED_COLOR_RED)
  {

    write_value = 0x00;
    // pull low OUT6-OUT9
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x02, &write_value, 1);

    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x21, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x22, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x24, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x25, &write_value, 1);
    // set led red
    // write_value = 0x20;
    write_value = 0x8F;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x20, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x23, &write_value, 1);

    // pull up OUT6-OUT9
    write_value = 0x3C;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x02, &write_value, 1);
  }
  else if (color == LED_COLOR_WHITE)
  {
    // pull low OUT6-OUT9
    write_value = 0x00;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x02, &write_value, 1);
    // set led white

    // Red
    write_value = 0x8C;
    // write_value = 0x28;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x20, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x23, &write_value, 1);

    // Green
    write_value = 0x1C;
    // write_value = 0xA;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x21, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x24, &write_value, 1);

    // Blue
    write_value = 0x34;
    // write_value = 0xF;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x22, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x25, &write_value, 1);
    // pull up OUT6-OUT9
    write_value = 0x3C;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x02, &write_value, 1);
  }
  else if (color == LED_COLOR_BLUE)
  {
    // pull low OUT6-OUT9
    write_value = 0x00;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x02, &write_value, 1);
    // set led blue
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x20, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x21, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x23, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x24, &write_value, 1);
    // write_value = 0x20;
    write_value = 0x8F;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x22, &write_value, 1);
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x25, &write_value, 1);
    // pull up OUT6-OUT9
    write_value = 0x3C;
    i2c_memory_write(Led_I2C1_Handle, LED_W_ADDR, 0x02, &write_value, 1);
  }
}
