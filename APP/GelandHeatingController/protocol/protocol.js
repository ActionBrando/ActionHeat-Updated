export const CMD_ID = {
  CMD_ID_SET_SWITCH: 0,
  CMD_ID_GET_SWITCH: 1,
  CMD_ID_SET_LEVEL: 2,
  CMD_ID_GET_LEVEL: 3,
  CMD_ID_GET_BATTERY: 4,
  CMD_ID_SET_TIME: 5,
  CMD_ID_GET_TIME: 6,
  CMD_ID_SET_TEMP: 7,
  CMD_ID_GET_TEMP: 8,
  CMD_ID_GET_REAL_TEMP: 9,
  CMD_ID_SET_LED_MODE: 10,
  CMD_ID_GET_LED_MODE: 11,
  CMD_ID_GET_LEFT_TIME: 12,
  CMD_ID_SET_PWM_LEVEL: 13,
  CMD_ID_GET_PWM_LEVEL: 14,
  CMD_ID_GET_VERSION: 15,
  CMD_ID_GET_SUB_BATTERY: 16,
  CMD_ID_SET_BT_NAME: 17,
};

export const CMD_DIR = {
  CMD_SEND: 1,
  CMD_REV: 2,
};

export const CMD_HEAD = "4347";

export function encodeCMD(data) {
  let cmd =
    CMD_HEAD +
    decimalToHexadecimal(data.cmdId) +
    decimalToHexadecimal(data.cmdDir) +
    decimalToHexadecimal(data.cmdLen) +
    decimalToHexadecimal(data.cmdData);
  return cmd;
}

export function decodeCMD(data) {
  let cmd = {
    cmdId: parseInt(data.slice(4, 6), 16),
    cmdDir: parseInt(data.slice(6, 8), 16),
    cmdLen: parseInt(data.slice(8, 10), 16),
  };
  cmd.cmdData = parseInt(
    data
      .slice(10, 10 + 2 * cmd.cmdLen)
      .match(/.{1,2}/g)
      .reverse()
      .join(""),
    16
  );
  return cmd;
}

//设置蓝牙开关
export function setBTSwitch(enable) {
  let data = {
    cmdId: CMD_ID.CMD_ID_SET_SWITCH,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 1,
    cmdData: enable ? 1 : 0,
  };
  return encodeCMD(data);
}

//读取蓝牙开关状态
export function getBTSwitch() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_SWITCH,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

//设置蓝牙加热档位
export function setBTLevel(level) {
  let data = {
    cmdId: CMD_ID.CMD_ID_SET_LEVEL,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 1,
    cmdData: level,
  };
  return encodeCMD(data);
}

//读取蓝牙加热档位
export function getBTLevel() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_LEVEL,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

//读取蓝牙电量
export function getBTBattery() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_BATTERY,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

//设置PWM档位
export function setPWMLevel(level) {
  let data = {
    cmdId: CMD_ID.CMD_ID_SET_PWM_LEVEL,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 1,
    cmdData: level,
  };
  return encodeCMD(data);
}

//读取PWM档位
export function getPWMLevel() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_PWM_LEVEL,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

//设置LED状态
export function setLEDStatus(enable) {
  let data = {
    cmdId: CMD_ID.CMD_ID_SET_LED_MODE,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 1,
    cmdData: enable,
  };
  return encodeCMD(data);
}

//读取LED状态
export function getLEDStatus() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_LED_MODE,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

export function setHeatingTemp(temp) {
  let data = {
    cmdId: CMD_ID.CMD_ID_SET_HEATING_TEMP,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 1,
    cmdData: temp,
  };
  return encodeCMD(data);
}

export function getHeatingTemp() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_HEATING_TEMP,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

export function getCurTemp() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_CUR_TEMP,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

export function getVersion() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_VERSION,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

export function getLeftTime() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_LEFT_TIME,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

export function getSubBattery() {
  let data = {
    cmdId: CMD_ID.CMD_ID_GET_SUB_BATTERY,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: 0,
    cmdData: 0,
  };
  return encodeCMD(data);
}

export function setBTName(name) {
  let data = {
    cmdId: CMD_ID.CMD_ID_SET_BT_NAME,
    cmdDir: CMD_DIR.CMD_SEND,
    cmdLen: name.length,
    cmdData: arrayToHex([name]),
  };
  return encodeCMD(data);
}

function decimalToHexadecimal(decimal) {
  const hexadecimal = decimal.toString(16).toUpperCase();
  return hexadecimal.padStart(2, "0");
}

function arrayToHex(array) {
  return array
    .map((item) => {
      if (typeof item === "number") {
        // 直接转换数字到16进制，去除前缀0x，并转大写,不足两位补0
        return item.toString(16).toUpperCase().padStart(2, "0");
      } else if (typeof item === "string") {
        // 将字符串每个字符转换为ASCII码的16进制表示
        return [...item]
          .map((char) => char.charCodeAt(0).toString(16).toUpperCase())
          .join("");
      }
    })
    .join(""); // 将转换后的数组元素拼接成一个字符串
}
