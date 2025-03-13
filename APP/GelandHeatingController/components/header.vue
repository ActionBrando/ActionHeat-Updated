<template>
  <view class="hi-columns" style="padding: 0 40rpx; box-sizing: border-box">
    <view class="hi-columns bar" style="width: 100%"></view>
    <view class="hi-rows hi-flex-vcenter hi-space-between head">
      <view class="hi-rows hi-flex-vcenter">
        <image
          v-if="showBack"
          @click="back"
          class="left-bluet-icon"
          src="/static/left-icon.png"
          style="margin-right: 30rpx"
          mode="widthFix"
        ></image>
        <image
          v-if="showBTIcon"
          @click="getMyDevices"
          class="left-bluet-icon"
          src="/static/left-bluet-icon.png"
          mode="widthFix"
        ></image>
        <!-- <image
          v-if="title != '' && title != 'Bluetooth'"
          @click="getMyDevices"
          class="left-bluet-icon"
          style="margin-left: 30rpx"
          src="/static/left-bluet-icon.png"
          mode="widthFix"
        ></image> -->
      </view>
      <image
        v-if="headerImg"
        style="width: 300rpx"
        :src="headerImg"
        mode="widthFix"
      ></image>
      <text
        v-if="headerText"
        @click="handleHeaderClick"
        class="font"
        style="font-size: 40rpx; color: #000"
        >{{ headerText }}</text
      >
      <!-- <image
        v-else
        style="width: 300rpx"
        src="/static/title-top.png"
        mode="widthFix"
      ></image> -->
      <image
        v-if="showMenu"
        @click="menuShow = true"
        class="menu-icons"
        src="/static/menu-icons.png"
      ></image>
    </view>
    <view
      class="hi-columns hi-flex-center hi-flex-vcenter"
      style="padding-top: 0; position: relative; z-index: 999"
    >
      <text
        v-if="title"
        class="font"
        style="
          font-size: 60rpx;
          background: -webkit-linear-gradient(left, #b3243d, #e44726);
          -webkit-background-clip: text;
          -webkit-text-fill-color: transparent;
        "
        >{{ title }}</text
      >
      <image
        v-if="titleImg"
        style="width: 300rpx"
        :src="titleImg"
        mode="widthFix"
      ></image>
    </view>

    <view v-if="menuShow" class="hi-columns mask" @click="menuShow = false">
      <view class="hi-columns bar" style="width: 100%"></view>
      <view class="hi-columns" style="height: 88rpx; width: 100%"></view>
      <view class="hi-rows hi-flex-vcenter hi-space-between">
        <view class="hi-columns"></view>
        <view class="hi-columns m-list">
          <text
            class="font t-title"
            @click.stop="items(index)"
            v-for="(item, index) in list"
            :key="index"
            >{{ item }}</text
          >
        </view>
      </view>
    </view>

    <view
      v-if="listShow"
      class="hi-columns hi-flex-center hi-flex-vcenter mask"
    >
      <view
        class="hi-columns"
        style="
          width: 690rpx;
          height: 80vh;
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
            style="font-size: 34rpx; color: #453185; font-weight: bold"
            >How It Use App</text
          >
          <image
            @click.stop="listShow = false"
            style="width: 56rpx; position: absolute; right: 30rpx"
            src="/static/close.png"
            mode="widthFix"
          ></image>
        </view>
        <scroll-view
          scroll-y="true"
          style="
            height: 73vh;
            width: 100%;
            padding-bottom: 30rpx;
            box-sizing: border-box;
          "
          class="hi-columns"
        >
          <view
            class="hi-columns"
            v-html="text"
            style="
              padding: 20rpx;
              box-sizing: border-box;
              text-align: center;
              color: #453185;
              line-height: 40rpx;
              font-size: 28rpx;
            "
          >
          </view>
        </scroll-view>
      </view>
    </view>
  </view>
</template>

<script>
export default {
  props: {
    showBack: {
      type: Boolean,
      default: false,
    },
    showBTIcon: {
      type: Boolean,
      default: false,
    },
    showMenu: {
      type: Boolean,
      default: false,
    },
    headerImg: {
      type: String,
      default: undefined,
    },
    titleImg: {
      type: String,
      default: "",
    },
    title: {
      type: String,
      default: "",
    },
    headerText: {
      type: String,
      default: "",
    },
  },
  data() {
    return {
      listShow: false,
      menuShow: false,
      list: ["How To Use", "My Devices", "Support", "Shop", "Home"],
      text: `PAIRING AN ITEM: <br>
This is filler copy the we need to be dynamic and editable. <br>
This is filler copy the we need to be dynamic <br>
and editable.  <br><br>

PAIRING MORE THAN 1 ITEM:<br>
This is filler copy the we need to be dynamic and editable. <br>
This is filler copy the we need to be dynamic <br>
and editable.  <br><br>

HAVING ISSUES WITH PAIRING?<br>
This is filler copy the we need to be dynamic and editable.<br>
This is filler copy the we need to be dynamic <br>
and editable.  <br><br>

TEMPERATURE CONTROL:<br>
This is filler copy the we need to be dynamic and editable. <br>
This is filler copy the we need to be dynamic <br>
and editable.  <br>
You can also do this manually by clicking the front of the <br>
control button on your garment 1- 3 times.<br><br>

GHOST MODE:<br>
Now you can stay hidden & stay warm at the same time<br>
with Ghost Mode! The Ghost Mode feature will switch the <br>
LED light on your control button will turn off, while maintaing <br>
your current heat setting. <br>
You can also do this manually by clicking the side of the <br>
control button on your garment <br><br>

BATTERY STATUS:<br>
This is filler copy the we need to be dynamic and editable. <br>
This is filler copy the we need to be dynamic <br>
and editable.  <br><br>

MAX RUN TIME:<br>
This is filler copy the we need to be dynamic and editable. <br>
This is filler copy the we need to be dynamic <br>
and editable.  `,
    };
  },
  onLoad() {},
  methods: {
    back() {
      uni.navigateBack({ delta: 1 });
    },
    getMyDevices() {
      uni.navigateTo({
        url: "/pages/btList/bluetoothList",
      });
    },
    items(index) {
      this.menuShow = false;
      if (index == 0) {
        this.listShow = true;
      } else if (index == 1) {
        uni.navigateTo({
          url: "/pages/btList/bluetoothList",
        });
      } else if (index == 2) {
        uni.navigateTo({
          url: "/pages/webview/webview?url=https://actionheat.com/pages/technology-new",
        });
      } else if (index == 3) {
        uni.navigateTo({
          url: "/pages/webview/webview?url=https://actionheat.com/collections",
        });
      } else {
        uni.reLaunch({
          url: "/pages/index/index",
        });
      }
    },

    handleHeaderClick() {
      this.$emit("onHeaderClick");
    },
  },
};
</script>

<style>
.head {
  height: 100rpx;
}
.left-bluet-icon {
  width: 56rpx;
  height: 56rpx;
}
.menu-icons {
  width: 44rpx;
  height: 66rpx;
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
.m-list {
  background-color: #fff;
  border-radius: 20rpx;
  padding: 20rpx 40rpx;
  margin-right: 30rpx;
  position: relative;
  width: 50%;
  z-index: 999999999;
}
.t-title {
  font-size: 36rpx;
  height: 46rpx;
  color: #453185;
  padding: 20rpx 0;
  text-align: center;
}
</style>
