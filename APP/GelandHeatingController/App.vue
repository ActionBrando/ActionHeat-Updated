<script>
import btManager from "./bluetooth/btManger";
import {
  getDeviceById,
  isSingleProduct,
  getPairedDeviceById,
  isValidBTName,
} from "./utils/utils.js";
import { mapState, mapActions } from "vuex";
const DEFAUT_MSG_QUEUE_DELAY = 10; // 默认消息队列处理间隔时间
export default {
  onShow: function () {},
  onHide: function () {},
  data() {
    return {
      isProcessing: false, // 是否正在处理消息
    };
  },
  created() {},
  onLaunch: function () {
    uni.$gBtManager.initManager();
    this.initBTMsgListener();
    // 监听处理消息事件
    uni.$eventHub.on("processGlobalMessage", this.processGlobalMessage);
  },
  computed: {
    ...mapState("bluetooth", ["connectedDeviceMap"]),
  },
  methods: {
    ...mapActions("bluetooth", [
      "addConnectedDevice",
      "removeConnectedDevice",
      "addFoundDevice",
    ]),
    getBTManager() {
      return uni.$gBtManager;
    },
    initBTMsgListener() {
      let that = this;
      uni.$gBtManager.addBluetoothDeviceFoundListener("global", (res) => {
        that.addFoundDevice({ payload: res.devices });
        that.autoConnectDevice(res.devices);
      });
      uni.$gBtManager.addBLEConnectionStateChangeListener("global", (res) => {
        //蓝牙连接状态变化
        if (res.connected) {
          that.addConnectedDevice({
            payload: { deviceId: res.deviceId },
          });
        } else {
          that.removeConnectedDevice({
            payload: { deviceId: res.deviceId },
          });
        }
      });
      uni.$gBtManager.addBLECharacteristicValueChangeListener(
        "global",
        (res) => {
          let matchDevice = getDeviceById(
            res.deviceId,
            that.connectedDeviceMap
          );
          if (matchDevice) {
            matchDevice.handleBTDataRecv(res, (cmdInfo) => {
              if (!isSingleProduct(matchDevice.deviceType)) {
                let pairDevice = getPairedDeviceById(
                  res.deviceId,
                  that.connectedDeviceMap
                );
                if (pairDevice) {
                  pairDevice.sendBTSetValueByCMD(cmdInfo);
                }
              }
            });
          }
        }
      );
      uni.$gBtManager.addNotifySuccessListener("global", (deviceId) => {
        let matchDevice = getDeviceById(deviceId, that.connectedDeviceMap);
        if (matchDevice) {
          if (!isSingleProduct(matchDevice.deviceType)) {
            let pairDevice = getPairedDeviceById(
              deviceId,
              that.connectedDeviceMap
            );
            if (pairDevice) {
              matchDevice.initDisplayByOtherDevice(pairDevice);
            } else {
              matchDevice.initDisplay();
            }
          } else {
            matchDevice.initDisplay();
          }
        }
      });
    },
    ///-----------------------------消息处理---------------------------------////

    processGlobalMessage() {
      if (this.isProcessing || uni.$globalMessageQueue.length === 0) {
        // 如果正在处理消息或消息队列为空，不执行处理
        return;
      }
      this.isProcessing = true;
      const message = uni.$globalMessageQueue.shift(); // 取出队首消息
      // 处理消息的逻辑
      this.processMsgImpl(message);
      // 使用 setTimeout 来设置间隔时间后再次触发处理消息事件
      setTimeout(() => {
        this.isProcessing = false;
        uni.$eventHub.emit("processGlobalMessage");
      }, message.msgDelay || DEFAUT_MSG_QUEUE_DELAY);
    },

    processMsgImpl(message) {
      switch (message.msgType) {
        case "send":
          //发送数据
          uni.$gBtManager.writeDataSync(
            message.msgData.deviceId,
            message.msgData.data
          );
          break;
        case "connect":
          //连接蓝牙
          uni.$gBtManager.toConnect(message.msgData.deviceId);
          break;
        case "startScan":
          //扫描蓝牙
          uni.$gBtManager.startScan();
          break;
        case "stopScan":
          //停止扫描蓝牙
          uni.$gBtManager.stopScan();
          break;
        default:
          break;
      }
    },
    ///-----------------------------消息处理---------------------------------////

    autoConnectDevice: function (devices) {
      let savedDevices = uni.getStorageSync("connectedDeviceMap");
      if (
        devices &&
        devices.length > 0 &&
        savedDevices &&
        savedDevices.length > 0
      ) {
        for (const device of devices) {
          if (!isValidBTName(device.localName || device.name)) {
            return;
          }
          let matchDevice = savedDevices.find((item) => {
            return item.deviceId == device.deviceId;
          });
          if (matchDevice && !matchDevice.manualDisconnect) {
            uni.addGlobalMessage({
              msgType: "connect",
              msgData: { deviceId: matchDevice.deviceId },
              msgDelay: 1500,
            });
          }
        }
      }
    },
  },
};
</script>

<style>
/*每个页面公共css */
page {
  height: 100%;
  width: 100%;
}

@font-face {
  font-family: "futura";
  src: url("/static/fonts/futura-pt-medium.ttf");
}

.font {
  font-family: "futura";
}

.bar {
  height: var(--status-bar-height);
}

.flex1 {
  flex: 1;
}

.hi-flex {
  display: flex !important;
}

.hi-rows {
  display: flex;
  flex-direction: row !important;
}

.hi-columns {
  display: flex;
  flex-direction: column !important;
}

.hi-wrap {
  display: flex;
  flex-wrap: wrap;
}

.hi-nowrap {
  display: flex;
  flex-wrap: nowrap !important;
}

.hi-space-between {
  display: flex;
  justify-content: space-between !important;
}

.hi-space-around {
  display: flex;
  justify-content: space-around !important;
}

.hi-flex-center {
  justify-content: center;
}

.hi-flex-top {
  align-items: flex-start;
}

.hi-flex-vcenter {
  align-items: center;
}
</style>
