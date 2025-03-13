

/*
 * INCLUDES (����ͷ�ļ�)
 */
#include <stdio.h>
#include <string.h>

#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"
#include "sys_utils.h"

#include "dev_info_service.h"

/*
 * MACROS (�궨��)
 */
/// Maximal length for Characteristic values - 128 bytes
#define DIS_VAL_MAX_LEN (128)

/*
 * CONSTANTS (��������)
 */

// Device Information Service UUID: 0x180A
static const uint8_t dis_svc_uuid[UUID_SIZE_2] = UUID16_ARR(DEVINFO_SERV_UUID);

/*
 * TYPEDEFS (���Ͷ���)
 */

/*
 * GLOBAL VARIABLES (ȫ�ֱ���)
 */

/*
 * LOCAL VARIABLES (���ر���)
 */

/*********************************************************************
 * Profile Attributes - Table
 * ÿһ���һ��attribute�Ķ��塣
 * ��һ��attributeΪService �ĵĶ��塣
 * ÿһ������ֵ(characteristic)�Ķ��壬�����ٰ�������attribute�Ķ��壻
 * 1. ����ֵ����(Characteristic Declaration)
 * 2. ����ֵ��ֵ(Characteristic value)
 * 3. ����ֵ������(Characteristic description)
 * �����notification ����indication �Ĺ��ܣ��������ĸ�attribute�Ķ��壬����ǰ�涨���������������һ������ֵ�ͻ�������(client characteristic configuration)��
 *
 */

static const gatt_attribute_t dis_att_table[] =
    {
        // Device information Service Declaration
        [DIS_IDX_SERVICE] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_PRIMARY_SERVICE_UUID)},
            GATT_PROP_READ,
            UUID_SIZE_2,
            (uint8_t *)dis_svc_uuid,
        },

#if ((DIS_FEATURES & DIS_MANUFACTURER_NAME_CHAR_SUP) == DIS_MANUFACTURER_NAME_CHAR_SUP)
        // Manufacturer name Declaration
        [DIS_IDX_MANUFACTURER_NAME_CHAR_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)},
            GATT_PROP_READ,
            0,
            NULL,
        },
        [DIS_IDX_MANUFACTURER_NAME_VALUE] = {
            {UUID_SIZE_2, UUID16_ARR(MANUFACTURER_NAME_UUID)},
            GATT_PROP_READ,
            DIS_VAL_MAX_LEN,
            NULL,
        },
#endif

#if ((DIS_FEATURES & DIS_MODEL_NB_STR_CHAR_SUP) == DIS_MODEL_NB_STR_CHAR_SUP)
        // Model Number String Declaration
        [DIS_IDX_MODEL_NUMBER_CHAR_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)},
            GATT_PROP_READ,
            0,
            NULL,
        },
        [DIS_IDX_MODEL_NUMBER_VALUE] = {
            {UUID_SIZE_2, UUID16_ARR(MODEL_NUMBER_UUID)},
            GATT_PROP_READ,
            DIS_VAL_MAX_LEN,
            NULL,
        },
#endif

#if ((DIS_FEATURES & DIS_SERIAL_NB_STR_CHAR_SUP) == DIS_SERIAL_NB_STR_CHAR_SUP)
        // Serial Number String Declaration
        [DIS_IDX_SERIAL_NUMBER_CHAR_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)},
            GATT_PROP_READ,
            0,
            NULL,
        },
        [DIS_IDX_SERIAL_NUMBER_VALUE] = {
            {UUID_SIZE_2, UUID16_ARR(SERIAL_NUMBER_UUID)},
            GATT_PROP_READ,
            DIS_VAL_MAX_LEN,
            NULL,
        },
#endif

#if ((DIS_FEATURES & DIS_HARD_REV_STR_CHAR_SUP) == DIS_HARD_REV_STR_CHAR_SUP)
        // Hardware Revision String Declaration
        [DIS_IDX_HARDWARE_REVISION_CHAR_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)},
            GATT_PROP_READ,
            0,
            NULL,
        },
        [DIS_IDX_HARDWARE_REVISION_VALUE] = {
            {UUID_SIZE_2, UUID16_ARR(HARDWARE_REV_UUID)},
            GATT_PROP_READ,
            DIS_VAL_MAX_LEN,
            NULL,
        },
#endif

#if ((DIS_FEATURES & DIS_FIRM_REV_STR_CHAR_SUP) == DIS_FIRM_REV_STR_CHAR_SUP)
        // Firmware Revision String Declaration
        [DIS_IDX_FIRMWARE_REVISION_CHAR_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)},
            GATT_PROP_READ,
            0,
            NULL,
        },
        [DIS_IDX_FIRMWARE_REVISION_VALUE] = {
            {UUID_SIZE_2, UUID16_ARR(FIRMWARE_REV_UUID)},
            GATT_PROP_READ,
            DIS_VAL_MAX_LEN,
            NULL,
        },
#endif

#if ((DIS_FEATURES & DIS_SW_REV_STR_CHAR_SUP) == DIS_SW_REV_STR_CHAR_SUP)
        // Software Revision String Declaration
        [DIS_IDX_SOFTWARE_REVISION_CHAR_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)},
            GATT_PROP_READ,
            0,
            NULL,
        },
        [DIS_IDX_SOFTWARE_REVISION_VALUE] = {
            {UUID_SIZE_2, UUID16_ARR(SOFTWARE_REV_UUID)},
            GATT_PROP_READ,
            DIS_VAL_MAX_LEN,
            NULL,
        },
#endif

#if ((DIS_FEATURES & DIS_SYSTEM_ID_CHAR_SUP) == DIS_SYSTEM_ID_CHAR_SUP)
        // System ID Declaration
        [DIS_IDX_SYSTEM_ID_CHAR_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)},
            GATT_PROP_READ,
            0,
            NULL,
        },
        // System ID Value
        [DIS_IDX_SYSTEM_ID_VALUE] = {
            {UUID_SIZE_2, UUID16_ARR(SYSTEM_ID_UUID)},
            GATT_PROP_READ,
            DIS_VAL_MAX_LEN,
            NULL,
        },
#endif

#if ((DIS_FEATURES & DIS_IEEE_CHAR_SUP) == DIS_IEEE_CHAR_SUP)
        // IEEE 11073-20601 Regulatory Certification Data List Characteristic Declaration
        [DIS_IDX_IEEE_11073_CHAR_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)},
            GATT_PROP_READ,
            0,
            NULL,
        },
        [DIS_IDX_IEEE_11073_VALUE] = {
            {UUID_SIZE_2, UUID16_ARR(IEEE_11073_CERT_DATA_UUID)},
            GATT_PROP_READ,
            DIS_VAL_MAX_LEN,
            NULL,
        },
#endif

#if ((DIS_FEATURES & DIS_PNP_ID_CHAR_SUP) == DIS_PNP_ID_CHAR_SUP)
        // PNP ID Declaration
        [DIS_IDX_PNP_ID_CHAR_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)},
            GATT_PROP_READ,
            0,
            NULL,
        },
        // PNP ID Value
        [DIS_IDX_PNP_ID_VALUE] = {
            {UUID_SIZE_2, UUID16_ARR(PNP_ID_UUID)},
            GATT_PROP_READ,
            DIS_VAL_MAX_LEN,
            NULL,
        },
#endif
};

/*********************************************************************
 * @fn      hid_gatt_op_cmp_handler
 *
 * @brief   Gatt operation complete handler.
 *
 *
 * @param   p_operation  - operation that has compeleted
 *
 * @return  none.
 */
void dis_gatt_op_cmp_handler(gatt_op_cmp_t *p_operation)
{
    if (p_operation->status == 0)
    {
    }
}

/*********************************************************************
 * @fn      dis_gatt_msg_handler
 *
 * @brief   Device information gatt message handler.
 *
 *
 * @param   p_msg  - messages from GATT layer.
 *
 * @return  none.
 */
static uint16_t dis_gatt_msg_handler(gatt_msg_t *p_msg)
{
    switch (p_msg->msg_evt)
    {
    case GATTC_MSG_READ_REQ:
        co_printf("dis read:%d\r\n", p_msg->att_idx);
#if ((DIS_FEATURES & DIS_MANUFACTURER_NAME_CHAR_SUP) == DIS_MANUFACTURER_NAME_CHAR_SUP)
        if (p_msg->att_idx == DIS_IDX_MANUFACTURER_NAME_VALUE)
        {
            memcpy(p_msg->param.msg.p_msg_data, DIS_MANUFACTURER_NAME, DIS_MANUFACTURER_NAME_LEN);
            return DIS_MANUFACTURER_NAME_LEN;
        }
#endif
#if ((DIS_FEATURES & DIS_MODEL_NB_STR_CHAR_SUP) == DIS_MODEL_NB_STR_CHAR_SUP)
        else if (p_msg->att_idx == DIS_IDX_MODEL_NUMBER_VALUE)
        {
            memcpy(p_msg->param.msg.p_msg_data, DIS_MODEL_NB_STR, DIS_MODEL_NB_STR_LEN);
            return DIS_MODEL_NB_STR_LEN;
        }
#endif
#if ((DIS_FEATURES & DIS_SERIAL_NB_STR_CHAR_SUP) == DIS_SERIAL_NB_STR_CHAR_SUP)
        else if (p_msg->att_idx == DIS_IDX_SERIAL_NUMBER_VALUE)
        {
            memcpy(p_msg->param.msg.p_msg_data, DIS_SERIAL_NB_STR, DIS_SERIAL_NB_STR_LEN);
            return DIS_SERIAL_NB_STR_LEN;
        }
#endif
#if ((DIS_FEATURES & DIS_HARD_REV_STR_CHAR_SUP) == DIS_HARD_REV_STR_CHAR_SUP)
        else if (p_msg->att_idx == DIS_IDX_HARDWARE_REVISION_VALUE)
        {
            memcpy(p_msg->param.msg.p_msg_data, DIS_HARD_REV_STR, DIS_HARD_REV_STR_LEN);
            return DIS_HARD_REV_STR_LEN;
        }
#endif
#if ((DIS_FEATURES & DIS_FIRM_REV_STR_CHAR_SUP) == DIS_FIRM_REV_STR_CHAR_SUP)
        else if (p_msg->att_idx == DIS_IDX_FIRMWARE_REVISION_VALUE)
        {
            memcpy(p_msg->param.msg.p_msg_data, DIS_FIRM_REV_STR, DIS_FIRM_REV_STR_LEN);
            return DIS_FIRM_REV_STR_LEN;
        }
#endif
#if ((DIS_FEATURES & DIS_SW_REV_STR_CHAR_SUP) == DIS_SW_REV_STR_CHAR_SUP)
        else if (p_msg->att_idx == DIS_IDX_SOFTWARE_REVISION_VALUE)
        {
            memcpy(p_msg->param.msg.p_msg_data, DIS_SW_REV_STR, DIS_SW_REV_STR_LEN);
            return DIS_SW_REV_STR_LEN;
        }
#endif
#if ((DIS_FEATURES & DIS_SYSTEM_ID_CHAR_SUP) == DIS_SYSTEM_ID_CHAR_SUP)
        else if (p_msg->att_idx == DIS_IDX_SYSTEM_ID_VALUE)
        {
            memcpy(p_msg->param.msg.p_msg_data, DIS_SYSTEM_ID, DIS_SYSTEM_ID_LEN);
            return DIS_SYSTEM_ID_LEN;
        }
#endif
#if ((DIS_FEATURES & DIS_IEEE_CHAR_SUP) == DIS_IEEE_CHAR_SUP)
        else if (p_msg->att_idx == DIS_IDX_IEEE_11073_VALUE)
        {
            memcpy(p_msg->param.msg.p_msg_data, DIS_IEEE, DIS_IEEE_LEN);
            return DIS_IEEE_LEN;
        }
#endif
#if ((DIS_FEATURES & DIS_PNP_ID_CHAR_SUP) == DIS_PNP_ID_CHAR_SUP)
        else if (p_msg->att_idx == DIS_IDX_PNP_ID_VALUE)
        {
            memcpy(p_msg->param.msg.p_msg_data, DIS_PNP_ID, DIS_PNP_ID_LEN);
            return DIS_PNP_ID_LEN;
        }
#endif
        break;

    case GATTC_MSG_WRITE_REQ:

        break;

    case GATTC_MSG_CMP_EVT:
        dis_gatt_op_cmp_handler((gatt_op_cmp_t *)&(p_msg->param.op));
        break;

    default:
        break;
    }
    return 0;
}

/*********************************************************************
 * @fn      dis_gatt_add_service
 *
 * @brief   Simple Profile add GATT service function.
 *          ����GATT service��ATT�����ݿ����档
 *
 * @param   None.
 *
 *
 * @return  None.
 */
void dis_gatt_add_service(void)
{
    gatt_service_t dis_profie_svc;

    dis_profie_svc.p_att_tb = dis_att_table;
    dis_profie_svc.att_nb = DIS_IDX_NB;
    dis_profie_svc.gatt_msg_handler = dis_gatt_msg_handler;

    gatt_add_service(&dis_profie_svc);
}
