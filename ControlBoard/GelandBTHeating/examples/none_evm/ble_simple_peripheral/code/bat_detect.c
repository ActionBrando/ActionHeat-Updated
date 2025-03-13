#include "bat_detect.h"
#include "driver_timer.h"
#include "driver_pmu.h"
#include "driver_gpio.h"
#include "co_log.h"

#define MAX_DECODE_BIT_COUNT 7

static uint8_t cur_detect_state = 0; // 0-未检测到 1-起始码低电平 2-起始码高电平 3-数据位低电平 4-数据位高电平
uint8_t last_state = 1;              // 默认高电平
uint32_t last_timer_count = 0;
static uint32_t timer_count = 0;
static uint32_t io_key_value;
uint8_t low_level_state = 0;  // 0-无效 1-长 2-短
uint8_t high_level_state = 0; // 0-无效 1-长 2-短
uint8_t decode_result = 0;
uint8_t decode_bit_count = 0;
uint8_t last_bat_num = 0xFF;

uint8_t module_state = 0; // 0-off,1-on

static uint8_t flag_process_bat_event = 0;

void (*upload_bat)(uint8_t);

void timer1_isr(void)
{
    timer_int_clear(Timer1);
    timer_count++;
}

void pmu_gpio_isr(uint32_t pin_value)
{
    io_key_value = pin_value;
    flag_process_bat_event = 1;
}

static void handle_bat_event(uint8_t key_value)
{
    uint8_t cmd = 0;
    uint8_t value = 0;
    cmd = key_value >> 4;
    if (cmd != 1)
    {
        return;
    }
    value = key_value & 0x0F;
    if (value > 4)
    {
        return;
    }
    if (upload_bat != NULL)
    {
        upload_bat(value);
        last_bat_num = value;
    }
    // co_printf("bat num:%d\r\n", value);
}

uint8_t get_bat_num(void)
{
    return last_bat_num;
}

void bat_detect_init(void (*p_callback)(uint8_t))
{

    pmu_port_wakeup_func_set(GPIO_PORT_D, GPIO_PIN_0);

    upload_bat = p_callback;
    __SYSTEM_TIMER_CLK_ENABLE();

    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);
    timer_init(Timer1, 75, TIMER_DIV_16); // 48000000/16/75 = 40000Hz
    bat_detect_start();
}

void bat_detect_start(void)
{
    timer_start(Timer1);
    reset_state();
    module_state = 1;
}

void bat_detect_stop(void)
{
    timer_stop(Timer1);
    module_state = 0;
}

uint32_t get_timer_count(void)
{
    return timer_count;
}

void reset_state(void)
{
    cur_detect_state = 0;
    last_timer_count = 0;
    low_level_state = 0;
    high_level_state = 0;
    decode_result = 0;
    decode_bit_count = 0;
    last_state = 1;
}

void process_bat_event(void)
{
    uint8_t cur_state;
    if (!flag_process_bat_event)
    {
        return;
    }
    flag_process_bat_event = 0;
    cur_state = (io_key_value & GPIO_PD0) > 0 ? 1 : 0;
    if (cur_state == last_state)
    {
        return;
    }
    last_state = cur_state;

    if (module_state == 0)
    {
        return;
    }

    switch (cur_detect_state)
    {
    case 0: // 默认高电平状态
    {
        if (cur_state == 0)
        {
            cur_detect_state = 1;
            last_timer_count = get_timer_count();
        }
        break;
    }
    case 1: // 起始码低电平
    {
        if (cur_state == 1)
        {
            if (check_level_valid(0, last_timer_count, get_timer_count()))
            {
                last_timer_count = get_timer_count();
                cur_detect_state = 2;
            }
            else
            {
                // 电平异常 重新检测
                reset_state();
            }
        }
        break;
    }
    case 2: // 起始码高电平
    {
        if (cur_state == 0)
        {
            if (check_level_valid(1, last_timer_count, get_timer_count()))
            {
                last_timer_count = get_timer_count();
                cur_detect_state = 3;
            }
            else
            {
                // 电平异常 重新检测
                reset_state();
            }
        }
        break;
    }
    case 3: // 数据位低电平
    {
        if (cur_state == 1)
        {
            // 上升沿记录结束时间
            if (check_level_valid(2, last_timer_count, get_timer_count()))
            {
                uint8_t data_bit = decode_data_bit();
                if (data_bit == 0xFF)
                {
                    // 数据位异常 重新检测
                    reset_state();
                }
                else
                {
                    decode_result = (decode_result << 1) | data_bit;
                    decode_bit_count++;
                    if (decode_bit_count >= MAX_DECODE_BIT_COUNT)
                    {
                        // 解码完成
                        // 处理解码结果
                        handle_bat_event(decode_result);
                        reset_state();
                    }
                    else
                    {
                        last_timer_count = get_timer_count();
                    }
                }
            }
            else
            {
                // 电平异常 重新检测
                reset_state();
            }
        }
        else
        {
            // 下降沿，记录开始时间
            last_timer_count = get_timer_count();
        }
        break;
    }
    }
}

// level_type: 0-起始码低电平 1-起始码高电平 2-数据位低电平 3-数据位高电平
uint8_t check_level_valid(uint8_t level_type, uint32_t start_count, uint32_t end_count)
{
    uint32_t count;
    if (end_count >= start_count)
    {
        count = end_count - start_count;
    }
    else
    {
        count = (0xFFFFFFFF - start_count) + end_count + 1;
    }
    switch (level_type)
    {
    case 0: // 起始码低电平
    {
        if (count >= START_CODE_LOW_COUNT_MIN && count <= START_CODE_LOW_COUNT_MAX)
        {
            return 1;
        }
        break;
    }
    case 1: // 起始码高电平
    {
        if (count >= START_CODE_HIGH_COUNT_MIN && count <= START_CODE_HIGH_COUNT_MAX)
        {
            return 1;
        }
        break;
    }
    case 2: // 数据位低电平
    {
        if (count >= DATA_CODE_LONG_COUNT_MIN && count <= DATA_CODE_LONG_COUNT_MAX)
        {
            low_level_state = 1;
            return 1;
        }
        else if (count >= DATA_CODE_SHORT_COUNT_MIN && count <= DATA_CODE_SHORT_COUNT_MAX)
        {
            low_level_state = 2;
            return 1;
        }
        low_level_state = 0;
        break;
    }
    case 3: // 数据位高电平
    {
        if (count >= DATA_CODE_LONG_COUNT_MIN && count <= DATA_CODE_LONG_COUNT_MAX)
        {
            high_level_state = 1;
            return 1;
        }
        else if (count >= DATA_CODE_SHORT_COUNT_MIN && count <= DATA_CODE_SHORT_COUNT_MAX)
        {
            high_level_state = 2;
            return 1;
        }
        high_level_state = 0;
        break;
    }
    }
    return 0;
}

uint8_t decode_data_bit(void)
{
    // if (low_level_state == 1 && high_level_state == 2)
    // {
    //     return 1;
    // }
    // else if (low_level_state == 2 && high_level_state == 1)
    // {
    //     return 0;
    // }
    if (low_level_state == 1)
    {
        return 1;
    }
    else if (low_level_state == 2)
    {
        return 0;
    }
    return 0xFF;
}
