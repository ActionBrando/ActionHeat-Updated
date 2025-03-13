

/*
 * INCLUDES
 */
#include <stdbool.h>

#include "co_log.h"
#include "os_mem.h"

#include "gap_api.h"
#include "gatt_api.h"

#include "ble_simple_peripheral.h"
#include "protocol.h"
#include "temp_control.h"
#include "led_control.h"
#include "voltage_detect.h"
#include "currency_detect.h"
#include "app_config.h"
#include "driver_flash.h"
#include "bat_detect.h"

/*
 * MACROS (????)
 */
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL (LOG_LEVEL_INFO)
extern const char *app_tag;

/*
 * CONSTANTS (????????)
 */

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8_t adv_data[] =
    {
        // service UUID, to notify central devices what services are included
        0x03,                   // length of this data
        GAP_ADVTYPE_16BIT_MORE, // some of the UUID's, but not all
        0xFF,
        0xFE,
};

// GAP - Scan response data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
/*
static uint8_t scan_rsp_data[] =
{
    // complete name
    0x0D,   // length of this data
    GAP_ADVTYPE_LOCAL_NAME_COMPLETE,
    '7','v',' ','O','u','t','e','r','w','e','a','r',

    // Tx power level
    0x02,   // length of this data
    GAP_ADVTYPE_POWER_LEVEL,
    0,      // 0dBm
};
*/
/*
 * TYPEDEFS
 */

/*
 * GLOBAL VARIABLES
 */

uint8_t sp_svc_id = 0;
uint8_t ntf_char1_enable = 0;

static gatt_service_t simple_profile_svc;

/*
 * LOCAL VARIABLES
 */

// Simple GATT Profile Service UUID: 0xFFF0
const uint8_t sp_svc_uuid[] = UUID16_ARR(SP_SVC_UUID);

/******************************* Characteristic 1 defination *******************************/
// Characteristic 1 UUID: 0xFFF1
// Characteristic 1 data
#define SP_CHAR1_VALUE_LEN 20
uint8_t sp_char1_value[SP_CHAR1_VALUE_LEN] = {0};
// Characteristic 1 User Description
#define SP_CHAR1_DESC_LEN 17
const uint8_t sp_char1_desc[SP_CHAR1_DESC_LEN] = "Characteristic 1";

/******************************* Characteristic 2 defination *******************************/
// Characteristic 2 UUID: 0xFFF2
// Characteristic 2 data
#define SP_CHAR2_VALUE_LEN 20
uint8_t sp_char2_value[SP_CHAR2_VALUE_LEN] = {0};
// Characteristic 2 User Description
#define SP_CHAR2_DESC_LEN 17
const uint8_t sp_char2_desc[SP_CHAR2_DESC_LEN] = "Characteristic 2";

const gatt_attribute_t simple_profile_att_table[SP_IDX_NB] =
    {
        // Simple gatt Service Declaration
        [SP_IDX_SERVICE] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_PRIMARY_SERVICE_UUID)}, /* UUID */
            GATT_PROP_READ,                                       /* Permissions */
            UUID_SIZE_2,
            /* Max size of the value */ /* Service UUID size in service declaration */
            (uint8_t *)sp_svc_uuid,
            /* Value of the attribute */ /* Service UUID value in service declaration */
        },

        // Characteristic 1 Declaration
        [SP_IDX_CHAR1_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)}, /* UUID */
            GATT_PROP_READ,                                 /* Permissions */
            0,                                              /* Max size of the value */
            NULL,                                           /* Value of the attribute */
        },
        // Characteristic 1 Value
        [SP_IDX_CHAR1_VALUE] = {
            {UUID_SIZE_16, SP_CHAR1_TX_UUID}, /* UUID */
            GATT_PROP_WRITE_CMD,              /* Permissions */
            SP_CHAR1_VALUE_LEN,               /* Max size of the value */
            NULL,
            /* Value of the attribute */ /* Can assign a buffer here, or can be assigned in the application by user */
        },

        // // Characteristic 2 Declaration
        [SP_IDX_CHAR2_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)}, /* UUID */
            GATT_PROP_READ,                                 /* Permissions */
            0,                                              /* Max size of the value */
            NULL,                                           /* Value of the attribute */
        },
        // // Characteristic 2 Value
        [SP_IDX_CHAR2_VALUE] = {
            {UUID_SIZE_16, SP_CHAR2_RX_UUID}, /* UUID */
            GATT_PROP_NOTI,                   /* Permissions */
            SP_CHAR2_VALUE_LEN,               /* Max size of the value */
            NULL,
            /* Value of the attribute */ /* Can assign a buffer here, or can be assigned in the application by user */
        },

        [SP_IDX_CHAR2_CFG] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID)}, /* UUID */
            GATT_PROP_READ | GATT_PROP_WRITE,                     /* Permissions */
            2,                                                    /* Max size of the value */
            NULL,
            /* Value of the attribute */ /* Can assign a buffer here, or can be assigned in the application by user */
        },
};

/*
 * LOCAL FUNCTIONS
 */
static void sp_start_adv(void);
/*
 * EXTERN FUNCTIONS
 */

/*
 * PUBLIC FUNCTIONS
 */

/** @function group ble peripheral device APIs
 * @{
 */

/*********************************************************************
 * @fn      app_gap_evt_cb
 *
 * @brief   Application layer GAP event callback function. Handles GAP evnets.
 *
 * @param   p_event - GAP events from BLE stack.
 *
 *
 * @return  None.
 */
void app_gap_evt_cb(gap_event_t *p_event)
{
    switch (p_event->type)
    {
    case GAP_EVT_ADV_END:
    {
        LOG_INFO(app_tag, "adv_end,status:0x%02x\r\n", p_event->param.adv_end.status);
    }
    break;

    case GAP_EVT_ALL_SVC_ADDED:
    {
        LOG_INFO(app_tag, "All service added\r\n");
        sp_start_adv();
    }
    break;

    case GAP_EVT_SLAVE_CONNECT:
    {
        LOG_INFO(app_tag, "slave[%d],connect. link_num:%d\r\n", p_event->param.slave_connect.conidx, gap_get_connect_num());
    }
    break;

    case GAP_EVT_DISCONNECT:
    {
        LOG_INFO(app_tag, "Link[%d] disconnect,reason:0x%02X\r\n", p_event->param.disconnect.conidx, p_event->param.disconnect.reason);
        gap_start_advertising(0);
    }
    break;

    case GAP_EVT_LINK_PARAM_REJECT:
        LOG_INFO(app_tag, "Link[%d]param reject,status:0x%02x\r\n", p_event->param.link_reject.conidx, p_event->param.link_reject.status);
        break;

    case GAP_EVT_LINK_PARAM_UPDATE:
        LOG_INFO(app_tag, "Link[%d]param update,interval:%d,latency:%d,timeout:%d\r\n", p_event->param.link_update.conidx, p_event->param.link_update.con_interval, p_event->param.link_update.con_latency, p_event->param.link_update.sup_to);
        break;

    case GAP_EVT_PEER_FEATURE:
        LOG_INFO(app_tag, "peer[%d] feats ind\r\n", p_event->param.peer_feature.conidx);
        break;

    case GAP_EVT_MTU:
        LOG_INFO(app_tag, "mtu update,conidx=%d,mtu=%d\r\n", p_event->param.mtu.conidx, p_event->param.mtu.value);
        break;

    case GAP_EVT_LINK_RSSI:
        LOG_INFO(app_tag, "link rssi %d\r\n", p_event->param.link_rssi);
        break;

    case GAP_SEC_EVT_SLAVE_ENCRYPT:
        LOG_INFO(app_tag, "slave[%d]_encrypted\r\n", p_event->param.slave_encrypt_conidx);
        break;

    default:
        break;
    }
}

/*********************************************************************
 * @fn      sp_start_adv
 *
 * @brief   Set advertising data & scan response & advertising parameters and start advertising
 *
 * @param   None.
 *
 *
 * @return  None.
 */
static void sp_start_adv(void)
{
    uint8_t main_vol;
    // Set advertising parameters
    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 300;
    adv_param.adv_intv_max = 300;

    gap_set_advertising_param(&adv_param);

    // Set advertising data & scan response data
    gap_set_advertising_data(adv_data, sizeof(adv_data));

    main_vol = get_main_vol_val();
    process_bt_name(main_vol, DEVICE_TYPE);
    // gap_set_advertising_rsp_data(scan_rsp_data, sizeof(scan_rsp_data));
    //  Start advertising
    LOG_INFO(app_tag, "Start advertising...\r\n");
    gap_start_advertising(0);
}

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
void simple_peripheral_init(void)
{
    // set local device name
    // uint8_t main_vol = get_main_vol_val();
    // generate_bt_name_str(main_vol,DEVICE_TYPE);

    // Initialize security related settings.
    gap_security_param_t param =
        {
            .mitm = false,
            .ble_secure_conn = false,
            .io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
            .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
            .bond_auth = true,
            .password = 0,
        };

    gap_security_param_init(&param);

    gap_set_cb_func(app_gap_evt_cb);

    // mac_addr_t addr;
    // enum ble_addr_type addr_type;
    // gap_address_get(&addr, &addr_type);

    // Adding services to database
    sp_gatt_add_service();
    temp_control_init(upload_value);
    vol_detect_init(upload_detected_vol);
    cur_detect_init();
    bat_detect_init(upload_detected_bat);
}

/*********************************************************************
 * @fn      sp_gatt_msg_handler
 *
 * @brief   Simple Profile callback funtion for GATT messages. GATT read/write
 *          operations are handeled here.
 *
 * @param   p_msg       - GATT messages from GATT layer.
 *
 * @return  uint16_t    - Length of handled message.
 */
static uint16_t sp_gatt_msg_handler(gatt_msg_t *p_msg)
{
    switch (p_msg->msg_evt)
    {
    case GATTC_MSG_READ_REQ:
        break;
    case GATTC_MSG_WRITE_REQ:
    {
        if (p_msg->att_idx == SP_IDX_CHAR1_VALUE)
        {
            // memcpy(sp_char4_ccc,p_msg->param.msg.p_msg_data,2);
            protocol_decode(p_msg->param.msg.p_msg_data, p_msg->param.msg.msg_len);
        }
    }
    break;
    case GATTC_MSG_LINK_CREATE:
        break;
    case GATTC_MSG_LINK_LOST:
        // ntf_char1_enable = 0;
        break;
    default:
        break;
    }
    return p_msg->param.msg.msg_len;
}

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
void sp_gatt_add_service(void)
{
    simple_profile_svc.p_att_tb = simple_profile_att_table;
    simple_profile_svc.att_nb = SP_IDX_NB;
    simple_profile_svc.gatt_msg_handler = sp_gatt_msg_handler;

    sp_svc_id = gatt_add_service(&simple_profile_svc);
}

void ntf_data(uint8_t *data, uint16_t len)
{
    gatt_ntf_t ntf_att;
    ntf_att.att_idx = SP_IDX_CHAR2_VALUE;
    ntf_att.conidx = 0;
    ntf_att.svc_id = sp_svc_id;
    ntf_att.data_len = len;
    ntf_att.p_data = data;
    gatt_notification(ntf_att);
}

void process_bt_name(uint8_t vol, uint8_t device_type)
{
    uint8_t result[30];
    char volt_str[10];
    char bt_name[20];
    char name_str[20];
    uint8_t vol_int;
    uint8_t len;
    uint8_t i;
    uint8_t save_len = 0;

    if (vol > 59)
    {
        vol_int = 7;
    }
    else
    {
        vol_int = 5;
    }
    snprintf(volt_str, sizeof(volt_str), "%dv", vol_int);

    flash_read(CUS_LEN_ADDR, 1, &save_len);
    if (save_len > 0 && save_len != 0xFF)
    {
        // have save data
        flash_read(CUS_NAME_ADDR, save_len, (uint8_t *)(&bt_name));
        len = save_len + 3;
    }
    else
    {
        switch (device_type)
        {
        case 0:
            strcpy(bt_name, "Outerwear");
            len = 12;
            break;
        case 1:
            strcpy(bt_name, "Pants");
            len = 8;
            break;
        case 2:
            strcpy(bt_name, "Footwear");
            len = 11;
            break;
        case 3:
            strcpy(bt_name, "Gloves");
            len = 9;
            break;
        case 4:
            strcpy(bt_name, "Accessories");
            len = 14;
            break;
        default:
            break;
        }
    }

    snprintf(name_str, sizeof(name_str), "%s %s", volt_str, bt_name);
    result[0] = len + 1;
    result[1] = GAP_ADVTYPE_LOCAL_NAME_COMPLETE;
    for (i = 0; i < len; i++)
    {
        result[i + 2] = name_str[i];
    }
    result[len + 2] = 0x02;
    result[len + 3] = GAP_ADVTYPE_POWER_LEVEL;
    result[len + 4] = 0;
    gap_set_advertising_rsp_data(result, sizeof(uint8_t) * (len + 5));
}

/*************service protocol start******************/
/**
 **/
void protocol_decode(uint8_t *p_data, uint16_t length)
{
    uint8_t switch_on;
    uint8_t level;
    uint8_t led_mode;
    uint16_t battery;
    uint8_t main_vol;
    uint8_t bat_num;
    if (p_data == NULL)
    {
        return;
    }
    if (p_data[0] == 0x43 && p_data[1] == 0x47)
    {
        if (p_data[3] != 0x01)
        {
            // not write cmd
            return;
        }
        switch_on = get_heating_status();
        switch (p_data[2])
        {
        case CMD_ID_SET_SWITCH: // set switch
            set_heating_status(p_data[5]);
            return;
        case CMD_ID_GET_SWITCH: // get switch status
            switch_on = get_heating_status();
            send_data(CMD_ID_GET_SWITCH, &switch_on, 1);
            return;
        case CMD_ID_SET_BT_NAME:
            bat_detect_stop();
            // store new name
            flash_erase(CUS_NAME_ADDR, 0xFF);
            flash_write(CUS_NAME_ADDR, p_data[4], p_data + 5);
            flash_write(CUS_LEN_ADDR, 1, p_data + 4);
            main_vol = get_main_vol_val();
            process_bt_name(main_vol, DEVICE_TYPE);
            bat_detect_start();
            break;
        default:
            break;
        }
        if (switch_on == STATUS_ON)
        {
            switch (p_data[2])
            {
            case CMD_ID_GET_LEVEL: // get heating level
                level = get_heating_level();
                send_data(CMD_ID_GET_LEVEL, &level, 1);
                break;
            case CMD_ID_GET_BATTERY: // get battery
                battery = get_detected_vol();
                send_data(CMD_ID_GET_BATTERY, (uint8_t *)(&battery), 2);
                break;
            case CMD_ID_SET_LED_MODE:
                set_led_mode(p_data[5]);
                break;
            case CMD_ID_GET_LED_MODE:
                led_mode = get_led_mode();
                send_data(CMD_ID_GET_LED_MODE, &led_mode, 1);
                break;
            case CMD_ID_SET_LEVEL_DUTY:
                set_pwm_level(p_data[5]);
                break;
            case CMD_ID_GET_LEVEL_DUTY:
                level = get_heating_level();
                send_data(CMD_ID_GET_LEVEL_DUTY, &level, 1);
                break;
            case CMD_ID_GET_SUB_BATTERY:
                bat_num = get_bat_num();
                send_data(CMD_ID_GET_SUB_BATTERY, (uint8_t *)(&bat_num), 1);
                break;
            default:
                break;
            }
        }
    }
}

static void send_data(uint8_t cmd, uint8_t *data, uint8_t len)
{
    uint8_t *send_data_arr;
    if (!data || len < 0x01 || cmd > CMD_ID_SET_BT_NAME)
    {
        return;
    }
    send_data_arr = (uint8_t *)os_malloc(sizeof(uint8_t) * len + 0x5);
    send_data_arr[0] = 0x43;
    send_data_arr[1] = 0x47;
    send_data_arr[2] = cmd;
    send_data_arr[3] = 0x02;
    send_data_arr[4] = len;
    memcpy(send_data_arr + 5, data, len);
    ntf_data(send_data_arr, 0x5 + len);
    os_free(send_data_arr);
}

void upload_value(uint8_t cmd, uint8_t value)
{
    send_data(cmd, &value, 1);
}

void upload_detected_vol(uint16_t vol)
{
    send_data(CMD_ID_GET_BATTERY, (uint8_t *)(&vol), 2);
}

void upload_detected_bat(uint8_t bat)
{
    send_data(CMD_ID_GET_SUB_BATTERY, (uint8_t *)(&bat), 1);
}

void triggle_power_level()
{
    uint8_t level = get_heating_level();
    uint8_t switch_on = get_heating_status();
    if (switch_on == STATUS_OFF)
    {
        return;
    }
    if (level == LOW_LEVEL)
    {
        set_pwm_level(HIGH_LEVEL);
    }
    else if (level == MID_LEVEL)
    {
        set_pwm_level(LOW_LEVEL);
    }
    else
    {
        set_pwm_level(MID_LEVEL);
    }
    upload_value(CMD_ID_GET_LEVEL, get_heating_level());
}

void triggle_led_mode()
{
    uint8_t led_mode = get_led_mode();
    uint8_t switch_on = get_heating_status();
    if (switch_on == STATUS_OFF)
    {
        return;
    }
    set_led_mode(!led_mode);
    upload_value(CMD_ID_GET_LED_MODE, !led_mode);
}

void triggle_heating_status()
{
    uint8_t switch_on = get_heating_status();
    set_heating_status(!switch_on);
    upload_value(CMD_ID_GET_SWITCH, !switch_on);
    upload_value(CMD_ID_GET_LED_MODE, !switch_on);
    upload_value(CMD_ID_GET_LEVEL, get_heating_level());
}

void abnormal_shutdown()
{
    set_heating_status(0);
    upload_value(CMD_ID_GET_SWITCH, 0);
    upload_value(CMD_ID_GET_LED_MODE, 0);
    upload_value(CMD_ID_GET_LEVEL, get_heating_level());
}

/*************service protocol end******************/
