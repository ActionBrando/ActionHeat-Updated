/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include <string.h>

#include "gap_api.h"
#include "gatt_api.h"
#include "ble_stack.h"
#include "app_config.h"

#include "jump_table.h"
#include "co_log.h"

#include "plf.h"
#include "driver_system.h"
#include "driver_pmu.h"
#include "driver_uart.h"
#include "driver_gpio.h"
#include "driver_adc.h"
#include "driver_pwm.h"
#include "driver_i2c.h"

#include "ble_simple_peripheral.h"
#include "key_control.h"
#include "led_control.h"
#include "temp_control.h"
#include "voltage_detect.h"
#include "key_control.h"
#include "currency_detect.h"
#include "bat_detect.h"
#include "driver_flash.h"

#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL (LOG_LEVEL_INFO)
const char *app_tag = "project";

#define SYSTEM_STACK_SIZE 0x800

/*
 * LOCAL VARIABLES
 */

I2C_HandleTypeDef I2C1_Handle;

__attribute__((section("stack_section"))) static uint32_t system_stack[SYSTEM_STACK_SIZE / sizeof(uint32_t)];

const struct jump_table_version_t _jump_table_version __attribute__((section("jump_table_3"))) =
    {
        .stack_top_address = &system_stack[SYSTEM_STACK_SIZE / sizeof(uint32_t)],
        .firmware_version = 0x00000000,
};

const struct jump_table_image_t _jump_table_image __attribute__((section("jump_table_1"))) =
    {
        .image_type = IMAGE_TYPE_APP,
        .image_size = 0x19000,
};

/*********************************************************************
 * @fn      user_entry_before_sleep_imp
 *
 * @brief   Before system goes to sleep mode, user_entry_before_sleep_imp()
 *          will be called, MCU peripherals can be configured properly before
 *          system goes to sleep, for example, some MCU peripherals need to be
 *          used during the system is in sleep mode.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void user_entry_before_sleep_imp(void)
{
    uart_putc_noint_no_wait(UART0, 's');
    co_delay_100us(1);

    pmu_set_pin_to_PMU(GPIO_PORT_A, (1 << GPIO_BIT_0));
    pmu_set_pin_dir(GPIO_PORT_A, (1 << GPIO_BIT_0), GPIO_DIR_IN);
    pmu_set_pin_pull(GPIO_PORT_A, (1 << GPIO_BIT_0), GPIO_PULL_NONE);
}

/*********************************************************************
 * @fn      user_entry_after_sleep_imp
 *
 * @brief   After system wakes up from sleep mode, user_entry_after_sleep_imp()
 *          will be called, MCU peripherals need to be initialized again,
 *          this can be done in user_entry_after_sleep_imp(). MCU peripherals
 *          status will not be kept during the sleep.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void user_entry_after_sleep_imp(void)
{
    pmu_set_pin_to_CPU(GPIO_PORT_A, (1 << GPIO_BIT_0));

    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_0, PORTA0_FUNC_UART0_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_1, PORTA1_FUNC_UART0_TXD);
    uart_init(UART0, 1152);
    fr_uart_enableIrq(UART0, Uart_irq_erbfi);

    uart_putc_noint_no_wait(UART0, 'w');
    co_delay_100us(1);

    NVIC_EnableIRQ(PMU_IRQn);
}

__attribute__((section("ram_code"))) void main_loop(void)
{
    while (1)
    {
        if (ble_stack_schedule_allow())
        {
            /*user code should be add here*/

            /* schedule internal stack event */
            ble_stack_schedule();
        }

        GLOBAL_INT_DISABLE();
        switch (ble_stack_sleep_check())
        {
        case 2:
        {
            ble_stack_enter_sleep();
        }
        break;
        default:
            break;
        }
        GLOBAL_INT_RESTORE();

        ble_stack_schedule_backward();

        process_key_event();
        process_vol_detect();
        process_cur_detect();
        process_bat_event();
    }
}

/*********************************************************************
 * @fn      proj_init
 *
 * @brief   Main entrancy of user application. This function is called after BLE stack
 *          is initialized, and all the application code will be executed from here.
 *          In that case, application layer initializtion can be startd here.
 *
 * @param   None.
 *
 *
 * @return  None.
 */

void my_adc_init(void)
{
    GPIO_InitTypeDef GPIO_Handle;
    adc_InitParameter_t ADC_InitParam;
    __SYSTEM_ADC_CLK_ENABLE();
    GPIO_Handle.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Handle.Mode = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull = GPIO_NOPULL;
    GPIO_Handle.Alternate = GPIO_FUNCTION_8;
    gpio_init(GPIO_D, &GPIO_Handle);

    ADC_InitParam.ADC_CLK_DIV = 5; // 5 24M/((x+1)*2)=2M
    ADC_InitParam.ADC_SetupDelay = 80;
    ADC_InitParam.ADC_Reference = ADC_REF_LDOIO;
    ADC_InitParam.FIFO_Enable = FIFO_DISABLE;
    adc_init(ADC_InitParam);
    adc_Channel_ConvertConfig(ADC_CHANNEL_0 | ADC_CHANNEL_1);
    adc_convert_enable();
}

void my_pwm_init(void)
{
    GPIO_InitTypeDef GPIO_Handle;
    struct_PWM_Config_t PWM_Config;

    __SYSTEM_PWM_CLK_ENABLE();

    /* init GPIO Alternate Function */
    GPIO_Handle.Pin = GPIO_PIN_3;
    GPIO_Handle.Mode = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull = GPIO_PULLDOWN;
    GPIO_Handle.Alternate = GPIO_FUNCTION_6;

    gpio_init(GPIO_C, &GPIO_Handle);
    PWM_Config.Prescale = 240000;
    PWM_Config.Period = 100;
    PWM_Config.Posedge = 0;
    PWM_Config.Negedge = 100;
    pwm_config(PWM_CHANNEL_3, PWM_Config);
}

void my_i2c_init(void)
{

    GPIO_InitTypeDef GPIO_Handle;
    __SYSTEM_GPIO_CLK_ENABLE();
    __SYSTEM_I2C0_CLK_ENABLE();
    __SYSTEM_I2C0_MASTER_CLK_SELECT_48M();

    /* I2C IO Init */
    GPIO_Handle.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_Handle.Mode = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_1;
    gpio_init(GPIO_A, &GPIO_Handle);

    /* I2C Init */
    I2C1_Handle.I2Cx = I2C0;
    I2C1_Handle.Init.I2C_Mode = I2C_MODE_MASTER_7BIT;
    I2C1_Handle.Init.SCL_HCNT = 500;
    I2C1_Handle.Init.SCL_LCNT = 500;
    i2c_init(&I2C1_Handle);
}

void power_key_init()
{
    GPIO_InitTypeDef GPIO_Handle;
    __SYSTEM_GPIO_CLK_ENABLE();
    // EXTI interrupt
    GPIO_Handle.Pin = GPIO_PIN_2;
    GPIO_Handle.Mode = GPIO_MODE_EXTI_IT_FALLING;
    GPIO_Handle.Pull = GPIO_PULLUP;
    gpio_init(GPIO_C, &GPIO_Handle);

    exti_interrupt_enable(EXTI_LINE18_PC2);
    exti_clear_LineStatus(EXTI_LINE18_PC2);
    NVIC_EnableIRQ(GPIO_IRQn);
}

void led_key_init()
{
    GPIO_InitTypeDef GPIO_Handle;
    __SYSTEM_GPIO_CLK_ENABLE();
    // EXTI interrupt
    GPIO_Handle.Pin = GPIO_PIN_1;
    GPIO_Handle.Mode = GPIO_MODE_EXTI_IT_FALLING;
    GPIO_Handle.Pull = GPIO_PULLUP;

    gpio_init(GPIO_D, &GPIO_Handle);
    exti_interrupt_enable(EXTI_LINE25_PD1);
    exti_clear_LineStatus(EXTI_LINE25_PD1);
    NVIC_EnableIRQ(GPIO_IRQn);
}

void misc_init(void)
{
    // key init
    power_key_init();
    led_key_init();
    //  adc init
    my_adc_init();
    // i2c init
    my_i2c_init();
    // pwm init
    my_pwm_init();
}

void proj_init(void)
{
    // Application layer initialization, can included bond manager init,
    // advertising parameters init, scanning parameter init, GATT service adding, etc.
    misc_init();
    key_control_init();
    led_control_init(&I2C1_Handle);
    simple_peripheral_init();
}

void setBTMac(void)
{
    mac_addr_t mac_addr;
    uint8_t device_mac[6] = {0};
    flash_read(0x60000, 6, device_mac); // 读取0x60000地址的6个字节做mac
    /* set local BLE address */
    if ((device_mac[0] != 0 && device_mac[1] != 0 && device_mac[2] != 0 && device_mac[3] != 0 && device_mac[4] != 0 && device_mac[5] != 0) && (device_mac[0] != 0xff && device_mac[1] != 0xff && device_mac[2] != 0xff && device_mac[3] != 0xff && device_mac[4] != 0xff && device_mac[5] != 0xff))
    {
        memcpy(mac_addr.addr, device_mac, 6);
    }
    else
    {
        system_get_unique_ID(device_mac); // 获取芯片唯一ID
        memcpy(mac_addr.addr, device_mac, 6);
    }
    for (uint8_t i = 0; i < 6; i++)
    {
        mac_addr.addr[i] = device_mac[5 - i];
    }
    gap_address_set(&mac_addr, BLE_ADDR_TYPE_PUBLIC); // 设置mac地址
}

/*********************************************************************
 * @fn      user_main
 *
 * @brief   Code to be executed for BLE stack initialization, Power mode
 *          configurations, etc.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void user_main(void)
{
    /* initialize log module */
    log_init();

    /* initialize PMU module at the beginning of this program */
    pmu_sub_init();

    /* set system clock */
    system_set_clock(SYSTEM_CLOCK_SEL);
    // co_printf("system_set_clock \r\n");

    /* set local BLE address */
    /*



    mac_addr_t mac_addr;
    mac_addr.addr[0] = 0xbd;
    mac_addr.addr[1] = 0xad;
    mac_addr.addr[2] = 0x10;
    mac_addr.addr[3] = 0x12;
    mac_addr.addr[4] = 0x21;
    mac_addr.addr[5] = 0x23;
    gap_address_set(&mac_addr, BLE_ADDR_TYPE_PRIVATE);
    */
    setBTMac();
    /* configure ble stack capabilities */
    ble_stack_configure(BLE_STACK_ENABLE_MESH,
                        BLE_STACK_ENABLE_CONNECTIONS,
                        BLE_STACK_RX_BUFFER_CNT,
                        BLE_STACK_RX_BUFFER_SIZE,
                        BLE_STACK_TX_BUFFER_CNT,
                        BLE_STACK_TX_BUFFER_SIZE,
                        BLE_STACK_ADV_BUFFER_SIZE,
                        BLE_STACK_RETENTION_RAM_SIZE,
                        BLE_STACK_KEY_STORAGE_OFFSET);
    /* initialize ble stack */
    ble_stack_init();
    /* initialize SMP */
    gap_bond_manager_init(BLE_BONDING_INFO_SAVE_ADDR, BLE_REMOTE_SERVICE_SAVE_ADDR, 8, true);
		proj_init();
    /* enter main loop */
    main_loop();
}
