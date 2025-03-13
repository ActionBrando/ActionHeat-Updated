

#ifndef PROTOCOL_H
#define PROTOCOL_H

/*
 */
#include <stdio.h>
#include <string.h>

enum
{
  CMD_ID_SET_SWITCH,      // 设置蓝牙开关
  CMD_ID_GET_SWITCH,      // 读取蓝牙开关状态
  CMD_ID_SET_LEVEL,       // 设置蓝牙加热档位
  CMD_ID_GET_LEVEL,       // 读取蓝牙加热档位
  CMD_ID_GET_BATTERY,     // 读取蓝牙电量
  CMD_ID_SET_TIME,        // 设置定时时间
  CMD_ID_GET_TIME,        // 读取定时时间
  CMD_ID_SET_TEMP,        // 设置温度
  CMD_ID_GET_TEMP,        // 读取温度
  CMD_ID_GET_REAL_TEMP,   // 读取实时温度
  CMD_ID_SET_LED_MODE,    // 设置LED模式
  CMD_ID_GET_LED_MODE,    // 读取LED模式
  CMD_ID_GET_LEFT_TIME,   // 获取加热剩余时间
  CMD_ID_SET_LEVEL_DUTY,  // set level duty
  CMD_ID_GET_LEVEL_DUTY,  // set level duty
  CMD_ID_GET_VERSION,     // 获取版本号
  CMD_ID_GET_SUB_BATTERY, // 读取专用充电宝电量
  CMD_ID_SET_BT_NAME,     // 设置蓝牙名称
};

#endif
