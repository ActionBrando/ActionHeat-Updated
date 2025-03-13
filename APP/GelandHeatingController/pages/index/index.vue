<template>
  <view class="hi-columns" style="height: 100%">
    <headers showBTIcon showMenu headerImg="/static/title-top.png"></headers>
    <view class="hi-columns hi-flex-vcenter hi-space-around flex1 index_bj">
      <view
        v-for="(item, index) in labelList"
        :key="index"
        @click="gotoDevicePage(item.url)"
        class="index-item hi-rows hi-flex-vcenter hi-space-between"
        :class="isActive(item.deviceType) ? 'active' : ''"
      >
        <text class="font title">{{ item.label }}</text>
        <image
          style="width: 126rpx"
          :src="isActive(item.deviceType) ? item.imgActive : item.img"
          mode="widthFix"
        ></image>
      </view>
    </view>
  </view>
</template>

<script>
import headers from "@/components/header.vue";
import { DEVICE_TYPE, startScan, stopScan } from "../../utils/utils";
import { mapState } from "vuex";
export default {
  components: {
    headers,
  },
  data() {
    return {
      labelList: [
        {
          label: "Outerwear",
          deviceType: DEVICE_TYPE.Outerwear,
          img: "/static/a01.png",
          imgActive: "/static/a01_active.png",
          status: 0, //0-未连接 1-已连接
          url: "/pages/device/device?deviceType=" + DEVICE_TYPE.Outerwear,
        },
        {
          label: "Pants",
          deviceType: DEVICE_TYPE.Pants,
          img: "/static/a02.png",
          imgActive: "/static/a02_active.png",
          status: 0, //0-未连接 1-已连接
          url: "/pages/device/device?deviceType=" + DEVICE_TYPE.Pants,
        },
        {
          label: "Footwear",
          deviceType: DEVICE_TYPE.Footwear,
          img: "/static/a03.png",
          imgActive: "/static/a03_active.png",
          status: 0, //0-未连接 1-已连接
          url: "/pages/device/device?deviceType=" + DEVICE_TYPE.Footwear,
        },
        {
          label: "Gloves",
          deviceType: DEVICE_TYPE.Gloves,
          img: "/static/a04.png",
          imgActive: "/static/a04_active.png",
          status: 0, //0-未连接 1-已连接
          url: "/pages/device/device?deviceType=" + DEVICE_TYPE.Gloves,
        },
        {
          label: "Accessories",
          deviceType: DEVICE_TYPE.Accessories,
          img: "/static/a05.png",
          imgActive: "/static/a05_active.png",
          status: 0, //0-未连接 1-已连接
          url: "/pages/device/device?deviceType=" + DEVICE_TYPE.Accessories,
        },
      ],
    };
  },
  onLoad() {
    this.initListener();
  },
  onShow() {
    setTimeout(() => {
      startScan();
    }, 1000);
  },
  computed: {
    ...mapState("bluetooth", ["connectedDeviceMap"]),
    isActive() {
      return function (deviceType) {
        if (!this.connectedDeviceMap) {
          return false;
        }
        return (
          this.connectedDeviceMap[deviceType] != null &&
          this.connectedDeviceMap[deviceType].length > 0
        );
      };
    },
  },
  methods: {
    gotoDevicePage(url) {
      uni.navigateTo({
        url: url,
      });
    },
    initListener: function () {
      let that = this;
      uni.$gBtManager.addNotifySuccessListener("index", function (e) {
        that.$forceUpdate();
      });
      uni.$gBtManager.addBLEConnectionStateChangeListener("index", (res) => {
        //蓝牙连接状态变化
        if (res.connected) {
          stopScan();
          startScan();
        }
      });
    },
  },
};
</script>

<style>
.index_bj {
  background: url(@/static/start_bj.png) no-repeat center;
  background-size: 100% 100%;
}

.index-item {
  padding-left: 30rpx;
  padding-right: 15rpx;
  box-sizing: border-box;
  width: 610rpx;
  height: 156rpx;
  border: 4rpx solid #fff;
  border-radius: 78rpx;
}
.index-item .title {
  font-size: 60rpx;
  /* font-weight: bold; */
  color: #fff;
}
.index-item.active {
  background-color: #fff;
}
.index-item.active .title {
  color: #b3243d;
}
</style>
