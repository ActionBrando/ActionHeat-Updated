

#ifndef SP_GATT_PROFILE_H
#define SP_GATT_PROFILE_H

/*
 * INCLUDES (����ͷ�ļ�)
 */
#include <stdio.h>
#include <string.h>
#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"

/*
 * MACROS (�궨��)
 */

/*
 * CONSTANTS (��������)
 */
// Simple Profile attributes index.
enum
{
    SP_IDX_SERVICE,

    SP_IDX_CHAR1_DECLARATION,
    SP_IDX_CHAR1_VALUE,
    SP_IDX_CHAR1_CFG,
    SP_IDX_CHAR1_USER_DESCRIPTION,

    SP_IDX_CHAR2_DECLARATION,
    SP_IDX_CHAR2_VALUE,
    SP_IDX_CHAR2_USER_DESCRIPTION,

    SP_IDX_NB,
};

// Simple GATT Profile Service UUID
#define SP_SVC_UUID 0xFFF0

#define SP_CHAR1_TX_UUID {0xb8, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}
#define SP_CHAR2_RX_UUID {0xba, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}

/*
 * TYPEDEFS (���Ͷ���)
 */

/*
 * GLOBAL VARIABLES (ȫ�ֱ���)
 */
extern const gatt_attribute_t simple_profile_att_table[];
/*
 * LOCAL VARIABLES (���ر���)
 */

/*
 * PUBLIC FUNCTIONS (ȫ�ֺ���)
 */
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
            GATT_PROP_READ | GATT_PROP_NOTI,  /* Permissions */
            SP_CHAR1_VALUE_LEN,               /* Max size of the value */
            NULL,
            /* Value of the attribute */ /* Can assign a buffer here, or can be assigned in the application by user */
        },

        // Characteristic 4 client characteristic configuration
        [SP_IDX_CHAR1_CFG] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID)}, /* UUID */
            GATT_PROP_READ | GATT_PROP_WRITE,                     /* Permissions */
            2,                                                    /* Max size of the value */
            NULL,
            /* Value of the attribute */ /* Can assign a buffer here, or can be assigned in the application by user */
        },

        // Characteristic 1 User Description
        [SP_IDX_CHAR1_USER_DESCRIPTION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID)}, /* UUID */
            GATT_PROP_READ,                                      /* Permissions */
            SP_CHAR1_DESC_LEN,                                   /* Max size of the value */
            (uint8_t *)sp_char1_desc,                            /* Value of the attribute */
        },

        // Characteristic 2 Declaration
        [SP_IDX_CHAR2_DECLARATION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID)}, /* UUID */
            GATT_PROP_READ,                                 /* Permissions */
            0,                                              /* Max size of the value */
            NULL,                                           /* Value of the attribute */
        },
        // Characteristic 2 Value
        [SP_IDX_CHAR2_VALUE] = {
            {UUID_SIZE_16, SP_CHAR2_RX_UUID}, /* UUID */
            GATT_PROP_READ | GATT_PROP_WRITE, /* Permissions */
            SP_CHAR2_VALUE_LEN,               /* Max size of the value */
            NULL,
            /* Value of the attribute */ /* Can assign a buffer here, or can be assigned in the application by user */
        },
        // Characteristic 2 User Description
        [SP_IDX_CHAR2_USER_DESCRIPTION] = {
            {UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID)}, /* UUID */
            GATT_PROP_READ,                                      /* Permissions */
            SP_CHAR2_DESC_LEN,                                   /* Max size of the value */
            (uint8_t *)sp_char2_desc,                            /* Value of the attribute */
        },

};

#endif
