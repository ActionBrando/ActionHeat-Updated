

/*
 * INCLUDES (����ͷ�ļ�)
 */
#include <stdio.h>
#include <string.h>
#include "mesh_api.h"
#include "mesh_sig_model_id.h"
#include "mesh_sig_msg.h"
#include "light_models.h"

/* MACROS (�궨��) */

/* CONSTANTS (��������)
 */

/*
 * TYPEDEFS (���Ͷ���)
 */

/*Prototypes */
static void mesh_light_onoff_msg_cb(mesh_model_msg_ind_t const *ind);
static void mesh_light_level_msg_cb(mesh_model_msg_ind_t const *ind);
static void mesh_light_lightness_msg_cb(mesh_model_msg_ind_t const *ind);

/*
 * GLOBAL VARIABLES (ȫ�ֱ���)
 */

/*
 * LOCAL VARIABLES (���ر���)
 */

static const mesh_model_t mesh_light_models[] =
    {
        {
            GEN_ON_OFF_SERVER_MODEL_ID, // model_id
            MODEL_TYPE_SIG,             // model_vendor
            0,                          // element_idx
            mesh_light_onoff_msg_cb,    // msg_cb
        },

        {
            GEN_LEVEL_SERVER_MODEL_ID,
            MODEL_TYPE_SIG,
            0,
            mesh_light_level_msg_cb,
        },

        {
            LIGHT_LIGHTNESS_SERVER_MODEL_ID,
            MODEL_TYPE_SIG,
            0,
            mesh_light_lightness_msg_cb,
        },
};

/*
 * LOCAL FUNCTIONS (���غ���)
 */
static void mesh_light_onoff_msg_cb(mesh_model_msg_ind_t const *ind)
{
}

static void mesh_light_level_msg_cb(mesh_model_msg_ind_t const *ind)
{
}

static void mesh_light_lightness_msg_cb(mesh_model_msg_ind_t const *ind)
{
}

/*
 * EXTERN FUNCTIONS (�ⲿ����)
 */

/*
 * PUBLIC FUNCTIONS (ȫ�ֺ���)
 */
void mesh_add_light_models(void)
{
    for (int i = 0; i < sizeof(mesh_light_models) / sizeof(mesh_model_t); i++)
        mesh_add_model(&mesh_light_models[i]);
}

void mesh_light_gen_onoff_set_msg(uint8_t element_idx, uint8_t onoff)
{
    static uint8_t tid = 0;
    mesh_publish_msg_t pub_msg;
    mesh_gen_onoff_set_unack_t gen_onoff_msg;

    gen_onoff_msg.onoff = onoff;
    gen_onoff_msg.tid = tid++;
    gen_onoff_msg.ttl = 0;
    gen_onoff_msg.delay = 0;

    pub_msg.element_idx = element_idx;
    pub_msg.model_id = GEN_ON_OFF_SERVER_MODEL_ID;
    pub_msg.opcode = GEN_ONOFF_SET_UNACK_MSG;
    pub_msg.msg_len = sizeof(mesh_gen_onoff_set_unack_t);
    // pub_msg.p_msg = (uint8_t *)&gen_onoff_msg;
    memcpy(pub_msg.msg, (uint8_t *)&gen_onoff_msg, pub_msg.msg_len);

    mesh_publish_msg(&pub_msg);
}
