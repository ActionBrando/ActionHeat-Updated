<template>
  <view class="hi-columns" style="width: 100%; height: 100%">
    <headers showBack showMenu headerImg="/static/bluetooth.png"></headers>
    <view class="hi-columns flex1 index_bj">
      <view
        class="hi-columns"
        style="padding: 20rpx 20rpx; padding-top: 60rpx; box-sizing: border-box"
      >
        <view class="hi-columns">
          <text
            class="font title"
            style="margin-bottom: 20rpx; font-size: 48rpx; color: #fff"
            >Connected Devices</text
          >

          <view
            v-for="(item, index) in connectedDeviceList"
            class="dev-item hi-rows hi-flex-vcenter hi-space-between"
          >
            <view class="hi-rows hi-flex-vcenter" @click="chgNameModal(item)">
              <image
                style="width: 30rpx"
                src="/static/bl.png"
                mode="widthFix"
              ></image>
              <text
                class="font"
                style="margin-left: 20rpx; font-size: 44rpx; color: #fff"
                >{{ item.deviceName + "-" + item.displayIdx }}</text
              >
            </view>
            <view class="btns font" @click="toConnectDevice(item, false)">
              Disconnect
            </view>
          </view>
        </view>

        <view class="hi-columns">
          <text
            class="font title"
            style="margin-bottom: 20rpx; font-size: 48rpx; color: #fff"
            >Available Devices</text
          >

          <view
            v-for="(item, index) in foundDeviceList"
            class="dev-item hi-rows hi-flex-vcenter hi-space-between"
          >
            <view class="hi-rows hi-flex-vcenter">
              <image
                style="width: 30rpx"
                src="/static/bl.png"
                mode="widthFix"
              ></image>
              <text
                class="font"
                style="margin-left: 20rpx; font-size: 44rpx; color: #fff"
                >{{ item.deviceName }}</text
              >
            </view>
            <view class="btns font" @click="toConnectDevice(item, true)">
              Connect
            </view>
          </view>
        </view>
      </view>
    </view>
  </view>
</template>

<script>
import headers from "@/components/header.vue";
import { mapState, mapActions } from "vuex";
import {
  deviceMap2Arr,
  DEVICE_TYPE,
  getRealDeviceName,
  startScan,
} from "../../utils/utils";
import { CMD_ID } from "../../protocol/protocol";
export default {
  computed: {
    ...mapState("bluetooth", ["connectedDeviceMap", "foundDeviceList"]),
    connectedDeviceList() {
      return deviceMap2Arr(this.connectedDeviceMap);
    },
    i18n() {
      return this.$t("index");
    },
  },
  components: {
    headers,
  },
  data() {
    return {};
  },
  onLoad() {
    startScan();
  },
  methods: {
    toConnectDevice(item, toConnect) {
      if (toConnect) {
        uni.$gBtManager.toConnect(item.deviceId);
      } else {
        item.manualDisconnect = true;
        uni.$gBtManager.toDisconnect(item.deviceId);
      }
    },
    chgNameModal: function (item) {
      let that = this;
      let actualName = getRealDeviceName(item.deviceName);
      uni.showModal({
        title: "Change Device Name",
        editable: true,
        placeholderText: actualName || "",
        content: "Please enter a name less than 15 characters.",
        showCancel: true,
        cancelText: "Cancel",
        confirmText: "Confirm",
        success: function (res) {
          if (res.confirm) {
            if (res && res.content && res.content != actualName) {
              if (res.content.length > 15) {
                uni.showToast({
                  title: "Please enter a name less than 15 characters.",
                  icon: "none",
                  duration: 2000,
                });
                return;
              } else if (res.content.length == 0) {
                uni.showToast({
                  title: "Please enter a name.",
                  icon: "none",
                  duration: 2000,
                });
                return;
              }
              that.processChgDeviceName(res.content, item);
            }
          } else if (res.cancel) {
          }
        },
        fail: function (res) {
          console.log("dlg fail", res);
        },
      });
    },
    processChgDeviceName: function (newName, deviceItem) {
      if (newName && deviceItem) {
        let typePrifix = "";
        switch (deviceItem.deviceType) {
          case DEVICE_TYPE.Outerwear:
            typePrifix = "O-";
            break;
          case DEVICE_TYPE.Pants:
            typePrifix = "P-";
            break;
          case DEVICE_TYPE.Footwear:
            typePrifix = "F-";
            break;
          case DEVICE_TYPE.Gloves:
            typePrifix = "G-";
            break;
          case DEVICE_TYPE.Accessories:
            typePrifix = "A-";
            break;
          default:
            break;
        }
        var volPrefix = deviceItem.deviceName.slice(0, 3);
        deviceItem.sendBtCmd(CMD_ID.CMD_ID_SET_BT_NAME, {
          volPrefix,
          newName: typePrifix + newName,
        });
      }
    },
  },
};
</script>

<style>
.index_bj {
  background: url(@/static/start_bj.png) no-repeat center;
  background-size: 100% 100%;
}
.dev-item {
  width: 700rpx;
  height: 100rpx;
  border: 3rpx solid #fff;
  border-radius: 50rpx;
  margin-left: 5rpx;
  padding-left: 30rpx;
  box-sizing: border-box;
  overflow: hidden;
  margin-bottom: 30rpx;
}
.dev-item .btns {
  width: 250rpx;
  height: 100rpx;
  line-height: 100rpx;
  text-align: center;
  color: #fff;
  font-size: 44rpx;
  border-left: 3rpx solid #fff;
}
.dev-item .btns.active {
  font-weight: bold;
  color: #b3243d;
  background-color: #fff;
}
</style>
