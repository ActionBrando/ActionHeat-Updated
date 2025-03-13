
#ifndef BLE_SIMPLE_PERIPHERAL_H
#define BLE_SIMPLE_PERIPHERAL_H

/*
 * INCLUDES (����ͷ�ļ�)
 */
#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"

/*
 * MACROS (�궨��)
 */

/*
 * CONSTANTS (��������)
 */
// Simple GATT Profile Service UUID
#define SP_SVC_UUID 0xFFF0

#define SP_CHAR1_TX_UUID {0xb8, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}
#define SP_CHAR2_RX_UUID {0xba, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}
#define SP_CHAR3_UUID 0xFFF3
#define SP_CHAR4_UUID 0xFFF4
#define SP_CHAR5_UUID 0xFFF5

/*
 * TYPEDEFS (���Ͷ���)
 */
enum
{
    GATT_SVC_IDX_SP,
    GATT_SVC_NUM
};

enum
{
    SP_IDX_SERVICE,

    SP_IDX_CHAR1_DECLARATION,
    SP_IDX_CHAR1_VALUE,
    // SP_IDX_CHAR1_CFG,
    // SP_IDX_CHAR1_USER_DESCRIPTION,

    SP_IDX_CHAR2_DECLARATION,
    SP_IDX_CHAR2_VALUE,
    SP_IDX_CHAR2_CFG,
    // SP_IDX_CHAR2_USER_DESCRIPTION,

    // SP_IDX_CHAR3_DECLARATION,
    // SP_IDX_CHAR3_VALUE,
    // SP_IDX_CHAR3_USER_DESCRIPTION,

    // SP_IDX_CHAR4_DECLARATION,
    // SP_IDX_CHAR4_VALUE,
    // SP_IDX_CHAR4_CFG,
    // SP_IDX_CHAR4_USER_DESCRIPTION,

    // SP_IDX_CHAR5_DECLARATION,
    // SP_IDX_CHAR5_VALUE,
    // SP_IDX_CHAR5_USER_DESCRIPTION,

    SP_IDX_NB,
};

/*
 * GLOBAL VARIABLES (ȫ�ֱ���)
 */

extern const gatt_attribute_t simple_profile_att_table[];

/*
 * LOCAL VARIABLES (���ر���)
 */

/*
 * LOCAL FUNCTIONS (���غ���)
 */

/*
 * EXTERN FUNCTIONS (�ⲿ����)
 */

/*
 * PUBLIC FUNCTIONS (ȫ�ֺ���)
 */

/** @function group ble peripheral device APIs (ble������ص�API)
 * @{
 */

void app_gap_evt_cb(gap_event_t *p_event);

/*********************************************************************
 * @fn      simple_peripheral_init
 *
 * @brief   Initialize simple peripheral profile, BLE related parameters.
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void simple_peripheral_init(void);
void protocol_decode(uint8_t *p_data, uint16_t length);
static void send_data(uint8_t cmd, uint8_t *data, uint8_t len);
void upload_value(uint8_t cmd, uint8_t value);
void upload_detected_vol(uint16_t vol);
void upload_detected_bat(uint8_t bat);
void triggle_power_level(void);
void triggle_led_mode(void);
void triggle_heating_status(void);
void process_bt_name(uint8_t vol, uint8_t device_type);
void abnormal_shutdown(void);

/*********************************************************************
 * @fn      sp_gatt_add_service
 *
 * @brief   Simple Profile add GATT service function.
 *          ����GATT service��ATT�����ݿ����档
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void sp_gatt_add_service(void);

#endif
