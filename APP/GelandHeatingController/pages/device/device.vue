<template>
  <view class="hi-columns" style="width: 100%; height: 100%">
    <headers
      :titleImg="titleImg"
      showMenu
      showBTIcon
      showBack
      :headerText="connectedName"
      @onHeaderClick="handleChgDevice"
    ></headers>
    <view class="hi-columns flex1 details_bj" style="margin-top: -50rpx">
      <view class="hi-columns hi-flex-center hi-flex-vcenter top-b">
        <view class="hi-columns" style="position: relative">
          <image style="width: 450rpx" :src="deviceImg" mode="widthFix"></image>
          <image
            style="width: 160rpx; position: absolute; top: 0; right: -30rpx"
            :src="
              btDevice.connectingStatus == 1
                ? leftIs5V
                  ? '/static/5v.png'
                  : '/static/7v.png'
                : ''
            "
            mode="widthFix"
          ></image>
        </view>
      </view>

      <view class="hi-columns hi-space-around" style="flex: 1 0 0">
        <view
          class="hi-columns hi-flex-vcenter hi-space-around"
          style="flex: 0.5 0 0; margin-top: 30rpx"
        >
          <view class="hi-rows kg-b">
            <view
              class="hi-columns hi-flex-center hi-flex-vcenter"
              @click="setBTSwitch(0)"
              :class="
                btDevice.connectingStatus == 1 && btDevice.heatingStatus == 0
                  ? 'active'
                  : ''
              "
              ><text style="font-size: 40rpx" class="font title"
                >OFF</text
              ></view
            >
            <view
              class="hi-columns hi-flex-center hi-flex-vcenter"
              @click="setBTSwitch(1)"
              :class="
                btDevice.connectingStatus == 1 && btDevice.heatingStatus == 1
                  ? 'active'
                  : ''
              "
              ><text style="font-size: 40rpx" class="font title">ON</text></view
            >
          </view>

          <text
            class="font title"
            style="font-size: 56rpx; color: #fff; margin-top: 60rpx"
            >Temperature Control</text
          >
        </view>
        <view
          class="hi-columns hi-flex-center hi-flex-vcenter"
          style="flex: 0.8 0 0"
        >
          <view
            class="hi-rows hi-flex-vcenter hi-space-between"
            style="width: 600rpx"
          >
            <view class="hi-columns hi-flex-vcenter">
              <image
                @click="btDevice.heatingPWMLevel != 0 ? setBTLevel(0) : ''"
                style="width: 120rpx"
                :src="getLevelBtnImg(0)"
                mode="widthFix"
              ></image>
              <text
                style="font-size: 32rpx; margin-top: 20rpx; color: #fff"
                class="font title"
                >LOW</text
              >
            </view>

            <view class="hi-columns hi-flex-vcenter">
              <image
                @click="btDevice.heatingPWMLevel != 1 ? setBTLevel(1) : ''"
                style="width: 120rpx"
                :src="getLevelBtnImg(1)"
                mode="widthFix"
              ></image>
              <text
                style="font-size: 32rpx; margin-top: 20rpx; color: #fff"
                class="font title"
                >MEDIUM</text
              >
            </view>
            <view class="hi-columns hi-flex-vcenter">
              <image
                @click="btDevice.heatingPWMLevel != 2 ? setBTLevel(2) : ''"
                style="width: 120rpx"
                :src="getLevelBtnImg(2)"
                mode="widthFix"
              ></image>
              <text
                style="font-size: 32rpx; margin-top: 20rpx; color: #fff"
                class="font title"
                >HIGH</text
              >
            </view>
          </view>
        </view>
        <view
          class="hi-columns hi-flex-vcenter hi-flex-center"
          style="flex: 1 0 0"
        >
          <view
            class="hi-rows hi-flex-vcenter hi-space-between"
            style="
              width: 600rpx;
              height: 120rpx;
              border: 4rpx solid #fff;
              border-radius: 20rpx;
              padding: 0 20rpx;
              box-sizing: border-box;
            "
          >
            <text class="font tilte" style="font-size: 40rpx; color: #fff"
              >Ghost Mode</text
            >
            <view
              class="hi-rows hi-flex-vcenter hi-space-around"
              style="width: 180rpx"
            >
              <image
                @click="btDevice.ledMode != 1 ? setLedMode(1) : ''"
                style="width: 60rpx"
                :src="
                  btDevice.ledMode != 1
                    ? '/static/led_on_default.png'
                    : '/static/led_on_active.png'
                "
                mode="widthFix"
              ></image>
              <image
                @click="btDevice.ledMode != 0 ? setLedMode(0) : ''"
                style="width: 60rpx"
                :src="
                  btDevice.ledMode != 0
                    ? '/static/led_off_default.png'
                    : '/static/led_off_active.png'
                "
                mode="widthFix"
              ></image>
            </view>
          </view>
          <view
            class="hi-rows hi-flex-vcenter hi-space-between"
            style="
              margin-top: 30rpx;
              width: 600rpx;
              height: 120rpx;
              border: 4rpx solid #fff;
              border-radius: 20rpx;
              padding: 0 20rpx;
              box-sizing: border-box;
            "
          >
            <text class="font tilte" style="font-size: 40rpx; color: #fff"
              >Battery Status</text
            >
            <view
              v-if="isSingleProduct(deviceType)"
              class="hi-rows hi-flex-vcenter hi-space-around"
              style="width: 180rpx"
            >
              <view
                v-for="index in leftBatDot"
                class="r-view active"
                :key="index"
              ></view>
              <view
                v-for="index in 4 - leftBatDot"
                class="r-view"
                :key="index"
              ></view>
            </view>
            <view v-else class="hi-rows hi-flex-vcenter">
              <view class="hi-columns" style="margin-right: 30rpx">
                <view
                  class="hi-rows hi-flex-vcenter hi-space-around"
                  style="width: 120rpx"
                >
                  <view
                    v-for="index in leftBatDot"
                    class="r-view active"
                    :key="index"
                  ></view>
                  <view
                    v-for="index in 4 - leftBatDot"
                    class="r-view"
                    :key="index"
                  ></view>
                </view>
                <view
                  class="hi-columns hi-flex-center hi-flex-vcenter"
                  style="padding: 10rpx 0"
                >
                  <text class="font title" style="color: #fff; font-size: 24rpx"
                    >LEFT</text
                  >
                </view>
              </view>

              <view class="hi-columns">
                <view
                  class="hi-rows hi-flex-vcenter hi-space-around"
                  style="width: 120rpx"
                >
                  <view
                    v-for="index in rightBatDot"
                    class="r-view active"
                    :key="index"
                  ></view>
                  <view
                    v-for="index in 4 - rightBatDot"
                    class="r-view"
                    :key="index"
                  ></view>
                </view>
                <view
                  class="hi-columns hi-flex-center hi-flex-vcenter"
                  style="padding: 10rpx 0"
                >
                  <text class="font title" style="color: #fff; font-size: 24rpx"
                    >RIGHT</text
                  >
                </view>
              </view>
            </view>
          </view>
        </view>
      </view>
    </view>
    <view
      v-show="chgDlgShow"
      class="hi-columns hi-flex-center hi-flex-vcenter mask"
    >
      <view
        class="hi-columns"
        style="
          width: 690rpx;
          height: 60vh;
          background: #fff;
          border-radius: 30rpx;
        "
      >
        <view
          class="hi-rows hi-flex-vcenter hi-flex-center"
          style="height: 7vh; width: 100%; position: relative"
        >
          <text
            class="font"
            style="font-size: 48rpx; color: #453185; font-weight: bold"
            >Choose Devices</text
          >
          <image
            @click.stop="chgDlgShow = false"
            style="width: 56rpx; position: absolute; right: 30rpx"
            src="/static/close.png"
            mode="widthFix"
          ></image>
        </view>
        <view
          v-for="item in connectedDeviceList"
          :key="item.deviceId"
          style="text-align: center"
        >
          <text
            @click="confirmChooseDevice(item)"
            class="font"
            style="font-size: 40rpx; color: #000; font-weight: bold"
            >{{ item.deviceName + "-" + item.displayIdx }}</text
          >
        </view>
      </view>
    </view>
  </view>
</template>

<script>
import headers from "@/components/header.vue";
import { CMD_ID } from "../../protocol/protocol";
import {
  calBatteryLevel,
  calSubBatteryLevel,
  CONNECTED_STATUS,
  DEVICE_TYPE,
  isSingleProduct,
} from "../../utils/utils";
import { mapState } from "vuex";
export default {
  watch: {
    "btDevice.battery": function (newVal, oldVal) {
      this.batteryVal = newVal;
    },
    "btDevice.subBattery": function (newVal, oldVal) {
      this.leftSubBatVal = newVal;
    },
    "rightBtDevice.battery": function (newVal, oldVal) {
      this.rightBatteryVal = newVal;
    },
    "rightBtDevice.subBattery": function (newVal, oldVal) {
      this.rightSubBatVal = newVal;
    },
    "rightBtDevice.heatingPWMLevel": function (newVal, oldVal) {
      this.$forceUpdate();
    },
    "rightBtDevice.heatingStatus": function (newVal, oldVal) {
      this.$forceUpdate();
    },
    "rightBtDevice.ledMode": function (newVal, oldVal) {
      this.$forceUpdate();
    },
  },
  components: {
    headers,
  },
  data() {
    return {
      deviceType: DEVICE_TYPE.Outerwear,
      titleImg: "",
      deviceImg: "",
      btDevice: {},
      rightBtDevice: {},
      batteryVal: 0,
      rightBatteryVal: 0,
      chgDlgShow: false,
      connectedName: "",
      leftSubBatVal: -1,
      rightSubBatVal: -1,
    };
  },
  onLoad(params) {
    this.deviceType = params.deviceType || DEVICE_TYPE.Footwear;
    this.initImage();
  },
  onShow() {
    this.initBtDevice();
    if (this.btDevice.connectingStatus == CONNECTED_STATUS.CONNECTED) {
      this.btDevice.initDisplay();
    }
  },
  computed: {
    ...mapState("bluetooth", ["connectedDeviceMap"]),
    connectedDeviceList() {
      return this.connectedDeviceMap[this.deviceType];
    },
    leftBatDot: function () {
      if (!this.btDevice.deviceId) {
        return 0;
      }
      if (this.leftSubBatVal >= 0 && this.leftSubBatVal <= 4) {
        return this.leftSubBatVal;
      }
      return calBatteryLevel(this.batteryVal);
    },
    rightBatDot: function () {
      if (!this.rightBtDevice.deviceId) {
        return 0;
      }
      if (this.rightSubBatVal >= 0 && this.rightSubBatVal <= 4) {
        return this.rightSubBatVal;
      }
      return calBatteryLevel(this.rightBatteryVal);
    },
    leftIs5V: function () {
      if (this.batteryVal <= 590) {
        return true;
      }
      return false;
    },
    rightIs5V: function () {
      if (this.rightBatteryVal <= 590) {
        return true;
      }
      return false;
    },
  },
  methods: {
    isSingleProduct,
    back() {
      uni.navigateBack();
    },
    gotoBluetoothList() {
      uni.navigateTo({
        url: "/pages/btList/bluetoothList",
      });
    },
    getLevelBtnImg(level) {
      if (level == 0) {
        if (this.btDevice.heatingPWMLevel == 0) {
          return "/static/low_level_active.png";
        }
        return "/static/low_level_default.png";
      } else if (level == 1) {
        if (this.btDevice.heatingPWMLevel == 1) {
          return "/static/mid_level_active.png";
        }
        return "/static/mid_level_default.png";
      } else if (level == 2) {
        if (this.btDevice.heatingPWMLevel == 2) {
          return "/static/mid_level_active.png";
        }
        return "/static/high_level_default.png";
      }
      return "";
    },
    initImage() {
      if (this.deviceType == DEVICE_TYPE.Outerwear) {
        this.titleImg = "/static/txt_title_outerwear.png";
        this.deviceImg = "/static/a01.png";
      } else if (this.deviceType == DEVICE_TYPE.Footwear) {
        this.titleImg = "/static/txt_title_footwear.png";
        this.deviceImg = "/static/a03.png";
      } else if (this.deviceType == DEVICE_TYPE.Gloves) {
        this.titleImg = "/static/txt_title_gloves.png";
        this.deviceImg = "/static/a04.png";
      } else if (this.deviceType == DEVICE_TYPE.Pants) {
        this.titleImg = "/static/txt_title_pants.png";
        this.deviceImg = "/static/a02.png";
      } else if (this.deviceType == DEVICE_TYPE.Accessories) {
        this.titleImg = "/static/txt_title_accessory.png";
        this.deviceImg = "/static/a05.png";
      } else {
        this.titleImg = "/static/txt_title_outerwear.png";
        this.deviceImg = "/static/a01.png";
      }
    },
    initBtDevice() {
      let matchDevices = this.connectedDeviceMap[this.deviceType];
      if (!matchDevices || matchDevices.length == 0) {
        this.btDevice = {};
        this.rightBtDevice = {};
        return;
      }
      this.btDevice = matchDevices[0];
      this.connectedName =
        this.btDevice.deviceName + "-" + this.btDevice.displayIdx;
      if (!isSingleProduct(this.deviceType)) {
        let pairDevice = matchDevices.find((item) => {
          return item.deviceId == this.btDevice.pairDeviceId;
        });
        if (pairDevice) {
          this.rightBtDevice = pairDevice;
        } else {
          this.rightBtDevice = {};
        }
      }
    },
    setBTSwitch(enable) {
      if (!this.btDevice.deviceId || !this.btDevice.checkBTConnectStatus()) {
        return;
      }
      if (this.btDevice.heatingStatus == enable) {
        return;
      }
      this.btDevice.sendBtCmd(CMD_ID.CMD_ID_SET_SWITCH, enable);
      if (enable) {
        this.btDevice.initDisplay();
      }
      if (
        this.rightBtDevice.deviceId &&
        this.rightBtDevice.checkBTConnectStatus()
      ) {
        this.rightBtDevice.sendBtCmd(CMD_ID.CMD_ID_SET_SWITCH, enable);
      }
    },
    setBTLevel(level) {
      if (!this.btDevice.checkBTStatus()) {
        return;
      }
      if (this.btDevice.heatingPWMLevel == level) {
        return;
      }
      this.btDevice.sendBtCmd(CMD_ID.CMD_ID_SET_PWM_LEVEL, level);
      if (this.rightBtDevice.deviceId && this.rightBtDevice.checkBTStatus()) {
        this.rightBtDevice.sendBtCmd(CMD_ID.CMD_ID_SET_PWM_LEVEL, level);
      }
    },
    setLedMode(mode) {
      if (!this.btDevice.checkBTStatus()) {
        return;
      }
      if (this.btDevice.ledMode == mode) {
        return;
      }
      this.btDevice.sendBtCmd(CMD_ID.CMD_ID_SET_LED_MODE, mode);
      if (this.rightBtDevice.deviceId && this.rightBtDevice.checkBTStatus()) {
        this.rightBtDevice.sendBtCmd(CMD_ID.CMD_ID_SET_LED_MODE, mode);
      }
    },
    handleChgDevice() {
      this.chgDlgShow = true;
    },
    confirmChooseDevice(item) {
      this.chgDlgShow = false;
      if (
        item.deviceId == this.btDevice.deviceId ||
        (this.rightBtDevice.deviceId &&
          item.deviceId == this.rightBtDevice.deviceId)
      ) {
        return;
      }
      this.btDevice = item;
      this.connectedName = item.deviceName + "-" + item.displayIdx;
      this.btDevice.initDisplay();
      let pairDevice = this.connectedDeviceMap[item.deviceType].find(
        (innerItem) => {
          return innerItem.deviceId == item.pairDeviceId;
        }
      );
      if (pairDevice) {
        this.rightBtDevice = pairDevice;
        this.rightBtDevice.initDisplay();
      } else {
        this.rightBtDevice = {};
      }
    },
  },
};
</script>

<style>
.details_bj {
  background: url(@/static/sing_bj.png) no-repeat;
  background-size: 100% 100%;
}
.top-b {
  width: 750rpx;
  background: url(@/static/goods_bj.png) no-repeat;
  background-size: 100% 100%;
  height: 500rpx;
}

.kg-b {
  width: 300rpx;
  height: 64rpx;
  border: 4rpx solid #fff;
  border-radius: 32rpx;
  overflow: hidden;
}
.kg-b > view {
  font-size: 28rpx;
  color: #fff;
  width: 50%;
}
.kg-b > view.active {
  background-color: #fff;
}
.kg-b > view text {
  color: #fff;
}
.kg-b > view.active text {
  background: -webkit-linear-gradient(left, #b3243d, #e44726);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
}
.r-view {
  width: 24rpx;
  height: 24rpx;
  border: 4rpx solid #ffff;
  border-radius: 50%;
  box-sizing: border-box;
}
.r-view.active {
  background-color: #fff;
}

.mask {
  width: 100%;
  height: 100%;
  position: fixed;
  top: 0;
  right: 0;
  left: 0;
  bottom: 0;
  background: rgba(0, 0, 0, 0.3);
  z-index: 999999;
}
</style>
