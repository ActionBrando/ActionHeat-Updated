export const DEVICE_TYPE = {
  Outerwear: 100,
  Pants: 101,
  Footwear: 108,
  Gloves: 109,
  Accessories: 110,
  Unknown: 999,
};

export const SEND_CMD_LOADING_DELAY = 1000; //发送命令后的loading延迟时间

export const DEVICE_BT_NAME = {
  Outerwear: "Outerwear",
  Pants: "Pants",
  Footwear: "Footwear",
  Gloves: "Gloves",
  Accessories: "Accessories",
  Unknown: "Unknown",
};

export const DEVICE_BT_SHORT_NAME = {
  Outerwear: "O",
  Pants: "P",
  Footwear: "F",
  Gloves: "G",
  Accessories: "A",
  Unknown: "Unknown",
};

export const CONNECTED_STATUS = {
  CONNECTED: 1,
  DISCONNECTED: 2,
};

export const HEATING_LEVEL = {
  NONE: -1,
  LOW: 0,
  MEDIUM: 1,
  HIGH: 2,
};

export const LED_MODE = {
  NONE: -1,
  OFF: 0,
  ON: 1,
};

export function getDeviceTypeByName(name) {
  let deviceType = 0;
  let actualName;
  if (name.startsWith("5v") || name.startsWith("7v")) {
    actualName = name.substr(3);
  } else if (name.startsWith("12v")) {
    actualName = name.substr(4);
  } else {
    actualName = name;
  }
  for (let key in DEVICE_BT_NAME) {
    if (
      DEVICE_BT_NAME[key] == actualName ||
      actualName.startsWith(DEVICE_BT_SHORT_NAME[key])
    ) {
      deviceType = DEVICE_TYPE[key];
      break;
    }
  }
  return deviceType;
}

export function getDeviceTypeByShortName(name) {}

//判断是否是有效的BT名称
export function isValidBTName(name) {
  //实现函数，如果以'5v'和'7v'开头，认为是有效的。
  if (!name) {
    return false;
  }
  let isValid = false;
  if (
    name.startsWith("5v") ||
    name.startsWith("7v") ||
    name.startsWith("12v")
  ) {
    isValid = true;
  }
  return isValid;
}

//判断是否是单个产品类型
export function isSingleProduct(productType) {
  if (
    productType == DEVICE_TYPE.Outerwear ||
    productType == DEVICE_TYPE.Pants ||
    productType == DEVICE_TYPE.Accessories
  ) {
    return true;
  }
  return false;
}

export function ab2hex(buffer) {
  const hexArr = Array.prototype.map.call(
    new Uint8Array(buffer),
    function (bit) {
      return ("00" + bit.toString(16)).slice(-2);
    }
  );
  return hexArr.join("");
}
export function hex2ArrayBuffer(hex_str) {
  let typedArray = new Uint8Array(
    hex_str.match(/[\da-f]{2}/gi).map(function (h) {
      return parseInt(h, 16);
    })
  );
  let buffer = typedArray.buffer;
  return buffer;
}
export function arrayBuffer2Hex(buffer) {
  const hexArr = Array.prototype.map.call(
    new Uint8Array(buffer),
    function (bit) {
      return ("00" + bit.toString(16)).slice(-2);
    }
  );
  return hexArr.join("");
}

export function getDeviceById(deviceId, deviceMap) {
  if (!deviceId || !deviceMap) {
    return null;
  }
  for (const key of Object.keys(deviceMap)) {
    let matchDevice = deviceMap[key].find((item) => item.deviceId == deviceId);
    if (matchDevice) {
      return matchDevice;
    }
  }
  return null;
}

export function getPairedDeviceById(deviceId, deviceMap) {
  if (!deviceId || !deviceMap) {
    return null;
  }
  for (const key of Object.keys(deviceMap)) {
    let matchDevice = deviceMap[key].find(
      (item) => item.pairDeviceId == deviceId
    );
    if (matchDevice) {
      return matchDevice;
    }
  }
  return null;
}

export function deviceMap2Arr(deviceMap) {
  let deviceArray = [];
  for (const key of Object.keys(deviceMap)) {
    deviceArray = deviceArray.concat(deviceMap[key]);
  }
  return deviceArray;
}

export function getBtIconByType(deviceType) {
  let icon = "";
  switch (deviceType) {
    case DEVICE_TYPE.Outerwear:
      icon = "/static/bt-jacket.png";
      break;
    case DEVICE_TYPE.Pants:
      icon = "/static/bt-pant.png";
      break;
    case DEVICE_TYPE.Footwear:
      icon = "/static/bt-socks.png";
      break;
    case DEVICE_TYPE.Gloves:
      icon = "/static/bt-gloves.png";
      break;
    case DEVICE_TYPE.Accessories:
      icon = "/static/bt-other.png";
      break;
    default:
      icon = "/static/bt-bticon.png";
      break;
  }
  return icon;
}

export function calBatteryLevel(outBattery) {
  let level = 0;
  let battery = outBattery + 40;
  if (battery < 590) {
    return 4;
  }
  if (battery >= 760) {
    return 4;
  } else if (battery > 730) {
    return 3;
  } else if (battery > 690) {
    return 2;
  } else if (battery > 600) {
    return 1;
  }
  return level;
}

export function calSubBatteryLevel(battery) {
  let level = 0;
  if (battery >= 388) {
    return 4;
  } else if (battery > 370) {
    return 3;
  } else if (battery > 358) {
    return 2;
  } else if (battery > 345) {
    return 1;
  }
  return level;
}

export function getRealDeviceName(fullName) {
  if (!fullName || fullName.length < 3) return "";
  let deviceName = fullName.substr(3);
  if (
    deviceName.startsWith("P-") ||
    deviceName.startsWith("F-") ||
    deviceName.startsWith("O-") ||
    deviceName.startsWith("G-") ||
    deviceName.startsWith("A-")
  ) {
    deviceName = deviceName.slice(2);
  }
  return deviceName;
}

export function startScan() {
  uni.addGlobalMessage({
    msgType: "startScan",
    msgData: {},
    msgDelay: 100,
  });
}

export function stopScan() {
  uni.addGlobalMessage({
    msgType: "stopScan",
    msgData: {},
    msgDelay: 500,
  });
}

export function addGlobalMessage(message) {
  uni.$globalMessageQueue.push(message);
  // 触发处理消息事件
  uni.$eventHub.emit("processGlobalMessage");
}

export function checkOS() {
  //#ifdef APP-PLUS
  if (plus && plus.os) {
    return plus.os.name;
  }
  //#endif
  return "";
}
