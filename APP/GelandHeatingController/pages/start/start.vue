<template>
  <view
    class="hi-columns hi-flex-center hi-flex-vcenter flex1"
    style="height: 100%"
  >
    <image
      style="width: 550rpx"
      src="/static/start_logo.png"
      mode="widthFix"
    ></image>
  </view>
</template>

<script>
export default {
  data() {
    return {};
  },
  onLoad() {
    setTimeout(() => {
      uni.reLaunch({
        url: "/pages/index/index",
      });
    }, 3000);
  },
  methods: {},
};
</script>

<style>
page {
  width: 100%;
  height: 100%;
  background: url(@/static/start_bj.png) no-repeat center;
  background-size: 100% 100%;
}
</style>
