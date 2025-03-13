import {
  CMD_ID,
  setBTSwitch,
  getBTSwitch,
  getBTLevel,
  getBTBattery,
  setPWMLevel,
  getPWMLevel,
  setLEDStatus,
  getLEDStatus,
  setHeatingTemp,
  decodeCMD,
  getHeatingTemp,
  getCurTemp,
  getVersion,
  getLeftTime,
  getSubBattery,
  setBTName,
} from "../protocol/protocol.js";

import {
  CONNECTED_STATUS,
  HEATING_LEVEL,
  LED_MODE,
  SEND_CMD_LOADING_DELAY,
  arrayBuffer2Hex,
} from "../utils/utils.js";

class btDevice {
  constructor(deviceName, deviceType, deviceId) {
    this.deviceType = deviceType; //设备类型
    this.deviceId = deviceId; //设备id
    this.deviceName = deviceName; //设备名称
    this.pairDeviceId = ""; //配对设备id
    this.connectingStatus = CONNECTED_STATUS.DISCONNECTED; //连接状态
    this.icon = ""; //图标
    this.heatingStatus = false; //加热状态
    this.heatingLevel = HEATING_LEVEL.NONE; //加热档位
    this.heatingPWMLevel = HEATING_LEVEL.NONE; //加热PWM档位
    this.ledMode = LED_MODE.NONE; //灯光模式
    this.heatingTime = 0; //加热时间
    this.heatingTimeLeft = 0; //剩余加热时间
    this.setTemp = 0; //设置温度
    this.curTemp = 0; //当前温度
    this.battery = 0; //电量
    this.subBattery = -1; //副电池电量
    this.is5V = false; //是否是5V
    this.manualDisconnect = false; //是否手动断开连接
    this.displayIdx = 0; //显示序号
    this.loadingHandler = null;
  }

  resetValue() {
    this.deviceId = ""; //设备id
    this.deviceName = ""; //设备名称
    this.pairDeviceId = ""; //配对设备id
    this.connectingStatus = CONNECTED_STATUS.DISCONNECTED; //连接状态
    this.icon = ""; //图标
    this.heatingStatus = false; //加热状态
    this.heatingLevel = HEATING_LEVEL.NONE; //加热档位
    this.heatingPWMLevel = HEATING_LEVEL.NONE; //加热PWM档位
    this.ledMode = LED_MODE.NONE; //灯光模式
    this.heatingTime = 0; //加热时间
    this.heatingTimeLeft = 0; //剩余加热时间
    this.setTemp = 0; //设置温度
    this.curTemp = 0; //当前温度
    this.battery = 0; //电量
    this.subBattery = -1; //副电池电量
    this.is5V = false; //是否是5V
    this.manualDisconnect = false; //是否手动断开连接
    this.displayIdx = 0; //显示序号
    this.handleBTDataRecv = this.handleBTDataRecv.bind(this);
  }

  resetOnDisconnect() {
    this.pairDeviceId = ""; //配对设备id
    this.connectingStatus = CONNECTED_STATUS.DISCONNECTED; //连接状态
    this.heatingStatus = false; //加热状态
    this.heatingLevel = HEATING_LEVEL.NONE; //加热档位
    this.heatingPWMLevel = HEATING_LEVEL.NONE; //加热PWM档位
    this.ledMode = LED_MODE.NONE; //灯光模式
  }

  setBtManager(manager) {
    this.btManager = manager;
  }

  destroyDevice() {
    this.btManager.removeCharacteristicValueChangeListener(this.deviceType);
    this.btManager.removeConnectionStateListener(this.deviceType);
    this.btManager.removeNotifySuccessListener(this.deviceType);
    this.btManager.disconnect(this.deviceType);
    this.resetValue();
  }

  //获取硬件端最新信息
  initAllStatus() {
    //发送指令
  }

  initDisplay = () => {
    let that = this;
    if (!this.checkBTConnectStatus()) {
      return;
    }
    setTimeout(() => {
      that.sendBtCmd(CMD_ID.CMD_ID_GET_SWITCH);
      that.sendBtCmd(CMD_ID.CMD_ID_GET_PWM_LEVEL);
      that.sendBtCmd(CMD_ID.CMD_ID_GET_LED_MODE);
      that.sendBtCmd(CMD_ID.CMD_ID_GET_SUB_BATTERY);
    }, 300);
    if (this.loadingHandler) {
      clearTimeout(this.loadingHandler);
    } else {
      uni.showLoading({
        title: "Processing...",
        mask: true,
      });
    }
    this.loadingHandler = setTimeout(() => {
      uni.hideLoading();
      this.loadingHandler = null;
    }, SEND_CMD_LOADING_DELAY);
  };

  initLevel = () => {
    if (!this.checkBTConnectStatus()) {
      return;
    }
    setTimeout(function () {
      this.sendBtCmd(CMD_ID.CMD_ID_GET_PWM_LEVEL);
    }, 300);
  };

  initDisplayByOtherDevice = (device) => {
    if (device) {
      this.sendBtCmd(CMD_ID.CMD_ID_SET_SWITCH, device.heatingStatus);
      this.sendBtCmd(CMD_ID.CMD_ID_SET_PWM_LEVEL, device.heatingPWMLevel);
      this.sendBtCmd(CMD_ID.CMD_ID_SET_LED_MODE, device.ledMode);
    }
    this.initDisplay();
  };

  // 发送蓝牙操作指令
  sendBtCmd(cmd, data) {
    if (!this.btManager) {
      return;
    }
    if (!this.checkBTConnectStatus()) {
      return;
    }
    let cmdData = "";
    switch (cmd) {
      case CMD_ID.CMD_ID_SET_SWITCH:
        if (this.heatingStatus == data) {
          return;
        }
        this.heatingStatus = data;
        if (!data) {
          this.heatingPWMLevel = HEATING_LEVEL.NONE;
          this.heatingLevel = HEATING_LEVEL.NONE;
          this.ledMode = LED_MODE.NONE;
        }
        cmdData = setBTSwitch(data);
        break;
      case CMD_ID.CMD_ID_GET_SWITCH:
        cmdData = getBTSwitch();
        break;
      case CMD_ID.CMD_ID_GET_LEVEL:
        cmdData = getBTLevel();
        break;
      case CMD_ID.CMD_ID_GET_BATTERY:
        cmdData = getBTBattery();
        break;
      case CMD_ID.CMD_ID_SET_LEVEL:
      case CMD_ID.CMD_ID_SET_PWM_LEVEL:
        this.heatingPWMLevel = data;
        this.heatingLevel = data;
        cmdData = setPWMLevel(data);
        break;
      case CMD_ID.CMD_ID_GET_PWM_LEVEL:
        cmdData = getPWMLevel();
        break;
      case CMD_ID.CMD_ID_SET_LED_MODE:
        if (this.ledMode == data) {
          return;
        }
        this.ledMode = data;
        cmdData = setLEDStatus(data);
        break;
      case CMD_ID.CMD_ID_GET_LED_MODE:
        cmdData = getLEDStatus();
        break;
      case CMD_ID.CMD_ID_SET_TEMP:
        if (this.setTemp == data) {
          return;
        }
        this.setTemp = data;
        cmdData = setHeatingTemp(data);
        break;
      case CMD_ID.CMD_ID_GET_TEMP:
        cmdData = getHeatingTemp();
        break;
      case CMD_ID.CMD_ID_GET_REAL_TEMP:
        cmdData = getCurTemp();
        break;
      case CMD_ID.CMD_ID_GET_VERSION:
        cmdData = getVersion();
        break;
      case CMD_ID.CMD_ID_GET_LEFT_TIME:
        cmdData = getLeftTime();
        break;
      case CMD_ID.CMD_ID_GET_SUB_BATTERY:
        cmdData = getSubBattery();
        break;
      case CMD_ID.CMD_ID_SET_BT_NAME:
        if (this.deviceName == data.volPrefix + data.newName) {
          return;
        }
        this.deviceName = data.volPrefix + data.newName;
        cmdData = setBTName(data.newName);
        break;
      default:
        break;
    }
    if (cmdData) {
      uni.addGlobalMessage({
        msgType: "send",
        msgData: { deviceId: this.deviceId, data: cmdData },
        msgDelay: 100,
      });
    }
  }

  sendBTSetValueByCMD = (e) => {
    switch (e.cmdId) {
      case CMD_ID.CMD_ID_GET_SWITCH:
        this.sendBtCmd(CMD_ID.CMD_ID_SET_SWITCH, e.cmdData);
        break;
      case CMD_ID.CMD_ID_GET_LEVEL:
      case CMD_ID.CMD_ID_GET_PWM_LEVEL:
        this.sendBtCmd(CMD_ID.CMD_ID_SET_PWM_LEVEL, e.cmdData);
        break;
      case CMD_ID.CMD_ID_GET_LED_MODE:
        this.sendBtCmd(CMD_ID.CMD_ID_SET_LED_MODE, e.cmdData);
        break;
      default:
        break;
    }
  };

  // 解析蓝牙返回的数据
  handleBTDataRecv(e, callback) {
    if (this.deviceId != e.deviceId) {
      return;
    }
    let data = arrayBuffer2Hex(e.value);
    let cmdInfo = decodeCMD(data);
    switch (cmdInfo.cmdId) {
      case CMD_ID.CMD_ID_GET_SWITCH:
        this.heatingStatus = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_LEVEL:
        this.heatingLevel = cmdInfo.cmdData;
        this.heatingPWMLevel = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_TIME:
        this.heatingTime = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_BATTERY:
        this.battery = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_LED_MODE:
        this.ledMode = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_TEMP:
        this.setTemp = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_REAL_TEMP:
        this.curTemp = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_LEFT_TIME:
        this.heatingTimeLeft = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_PWM_LEVEL:
        this.heatingPWMLevel = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_SUB_BATTERY:
        this.subBattery = cmdInfo.cmdData;
        break;
      case CMD_ID.CMD_ID_GET_VERSION:
        this.version = cmdInfo.cmdData;
        break;
      default:
        break;
    }
    if (callback) {
      callback(cmdInfo);
    }
  }

  checkBTConnectStatus = () => {
    if (this.connectingStatus == CONNECTED_STATUS.DISCONNECTED) {
      uni.showToast({
        title: "Please connect the device first",
        icon: "none",
      });
      return false;
    }
    return true;
  };

  checkBTConnectStatusNoWarn = () => {
    if (this.connectingStatus == CONNECTED_STATUS.DISCONNECTED) {
      return false;
    }
    return true;
  };

  setBTCharacteristicNotify = () => {
    this.btManager.notify(this.deviceId);
  };

  setDeviceId = (deviceId) => {
    this.deviceId = deviceId;
  };

  setDeviceName = (deviceName) => {
    this.deviceName = deviceName;
  };

  handleBTConnectStatusChange = (e) => {
    if (this.deviceId != e.deviceId) {
      return;
    }
    if (e.connected) {
      this.connectingStatus = CONNECTED_STATUS.CONNECTED;
      this.setBTCharacteristicNotify();
      this.initAllStatus();
    } else {
      this.connectingStatus = CONNECTED_STATUS.DISCONNECTED;
      if (!this.manualDisconnect) {
        uni.showToast({
          title: "Device disconnected",
          icon: "none",
        });
      }
      this.manualDisconnect = false;
    }
  };

  checkBTStatus = () => {
    if (!this.checkBTConnectStatus()) {
      return false;
    }
    if (this.heatingStatus == 0) {
      uni.showToast({
        title: "Please turn on the device first",
        icon: "none",
      });
      return false;
    }
    return true;
  };
}

export default btDevice;
