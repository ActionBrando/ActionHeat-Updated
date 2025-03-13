import {
  CONNECTED_STATUS,
  deviceMap2Arr,
  getBtIconByType,
  getDeviceTypeByName,
  isSingleProduct,
  isValidBTName,
} from "../../utils/utils";
import btDevice from "../../bluetooth/btDevice";

const state = {
  connectedDeviceMap: {},
  foundDeviceList: [],
};

const mutations = {
  saveConnectedDevice(state, payload) {
    state.connectedDeviceMap = {
      ...payload,
    };
    let savedDevices = uni.getStorageSync("connectedDeviceMap");
    let newDevices = deviceMap2Arr(payload);
    if (!savedDevices) {
      savedDevices = [];
    }
    newDevices.forEach((device) => {
      if (
        !savedDevices.some(
          (savedDevice) => savedDevice.deviceId === device.deviceId
        )
      ) {
        savedDevices.push(device);
      }
    });
    uni.setStorageSync("connectedDeviceMap", savedDevices);
  },

  removeSavedDevice(state, payload) {
    let savedDevices = uni.getStorageSync("connectedDeviceMap");
    if (savedDevices && savedDevices.length) {
      let matchDeviceIndex = savedDevices.findIndex((item) => {
        return item.deviceId == payload.deviceId;
      });
      if (matchDeviceIndex >= 0) {
        savedDevices.splice(matchDeviceIndex, 1);
      }
    }
    uni.setStorageSync("connectedDeviceMap", savedDevices);
  },

  cleanConnectedDevice(state) {
    state.connectedDeviceMap = {};
    uni.setStorageSync("connectedDeviceMap", []);
  },

  saveFoundDevice(state, payload) {
    state.foundDeviceList = [...payload];
  },

  cleanFoundDevice(state) {
    state.foundDeviceList = [];
  },
};

const actions = {
  addConnectedDevice({ commit, state, dispatch }, { payload, callback }) {
    if (!payload || !payload.deviceId) {
      return;
    }
    let matchFoundDevice = state.foundDeviceList.find((item) => {
      return item.deviceId == payload.deviceId;
    });
    if (!matchFoundDevice) {
      return;
    }
    let deviceType = getDeviceTypeByName(matchFoundDevice.deviceName);
    if (!deviceType) {
      return;
    }
    let matchTypeDevice = state.connectedDeviceMap[deviceType];
    if (matchTypeDevice) {
      let matchExistDevice = matchTypeDevice.find((item) => {
        return item.deviceId == payload.deviceId;
      });
      if (matchExistDevice) {
        //已添加不处理
        return;
      }
    } else {
      matchTypeDevice = [];
    }
    let newDevice = new btDevice(
      matchFoundDevice.deviceName,
      deviceType,
      payload.deviceId
    );
    newDevice.setBtManager(uni.$gBtManager);
    newDevice.connectingStatus = CONNECTED_STATUS.CONNECTED;
    newDevice.icon = getBtIconByType(deviceType);
    if (!isSingleProduct(newDevice.deviceType)) {
      //配对的设备，查找是否存在未配对的同类型设备
      let existDeviceIndex = matchTypeDevice.findIndex((item) => {
        return item.pairDeviceId == "";
      });
      if (existDeviceIndex >= 0) {
        //存在，配对
        newDevice.pairDeviceId = matchTypeDevice[existDeviceIndex].deviceId;
        matchTypeDevice[existDeviceIndex].pairDeviceId = newDevice.deviceId;
        newDevice.displayIdx = matchTypeDevice[existDeviceIndex].displayIdx;
      } else {
        let lastDevice = matchTypeDevice[matchTypeDevice.length - 1];
        newDevice.displayIdx = lastDevice ? lastDevice.displayIdx + 1 : 1;
      }
    } else {
      let lastDevice = matchTypeDevice[matchTypeDevice.length - 1];
      newDevice.displayIdx = lastDevice ? lastDevice.displayIdx + 1 : 1;
    }
    matchTypeDevice.push(newDevice);
    state.connectedDeviceMap[deviceType] = matchTypeDevice;
    commit("saveConnectedDevice", {
      ...state.connectedDeviceMap,
    });
    dispatch("removeFoundDevice", {
      payload: { deviceId: payload.deviceId },
    });
  },

  removeConnectedDevice({ commit, state }, { payload, callback }) {
    if (!payload || !payload.deviceId) {
      return;
    }
    for (const key of Object.keys(state.connectedDeviceMap)) {
      let matchDeviceIndex = state.connectedDeviceMap[key].findIndex((item) => {
        return item.deviceId == payload.deviceId;
      });
      if (matchDeviceIndex >= 0) {
        let matchDevice = state.connectedDeviceMap[key][matchDeviceIndex];
        if (matchDevice.pairDeviceId) {
          //配对设备
          let matchPairDeviceIndex = state.connectedDeviceMap[key].findIndex(
            (item) => {
              return item.deviceId == matchDevice.pairDeviceId;
            }
          );
          if (matchPairDeviceIndex >= 0) {
            state.connectedDeviceMap[key][matchPairDeviceIndex].pairDeviceId =
              "";
          }
        }
        state.connectedDeviceMap[key].splice(matchDeviceIndex, 1);
        state.foundDeviceList.push({
          deviceName: matchDevice.deviceName,
          deviceId: matchDevice.deviceId,
          icon: "/static/bt-bticon.png",
        });

        if (matchDevice.manualDisconnect) {
          commit("removeSavedDevice", { deviceId: matchDevice.deviceId });
        }
        matchDevice.resetOnDisconnect();
        commit("saveConnectedDevice", {
          ...state.connectedDeviceMap,
        });
        commit("saveFoundDevice", state.foundDeviceList);
        break;
      }
    }
  },

  addFoundDevice({ commit, state }, { payload, callback }) {
    if (!payload || !payload.length) {
      return;
    }
    let connectedList = deviceMap2Arr(state.connectedDeviceMap);
    for (const device of payload) {
      if (!isValidBTName(device.localName || device.name)) {
        continue;
      }
      let existConnectedDevice = connectedList.find((item) => {
        return item.deviceId == device.deviceId;
      });
      if (existConnectedDevice) {
        //已连接设备不添加
        continue;
      }
      let existDevice = state.foundDeviceList.find((item) => {
        return item.deviceId == device.deviceId;
      });
      if (!existDevice) {
        state.foundDeviceList.push({
          deviceName: device.localName || device.name,
          deviceId: device.deviceId,
          icon: "/static/bt-bticon.png",
        });
      } else {
        existDevice.deviceName = device.localName || device.name;
      }
    }
    commit("saveFoundDevice", state.foundDeviceList);
  },

  removeFoundDevice({ commit, state }, { payload, callback }) {
    if (!payload || !payload.deviceId) {
      return;
    }
    let matchDeviceIndex = state.foundDeviceList.findIndex((item) => {
      return item.deviceId == payload.deviceId;
    });
    if (matchDeviceIndex >= 0) {
      state.foundDeviceList.splice(matchDeviceIndex, 1);
      commit("saveFoundDevice", state.foundDeviceList);
    }
  },
};

export default {
  namespaced: true,
  state,
  mutations,
  actions,
};
