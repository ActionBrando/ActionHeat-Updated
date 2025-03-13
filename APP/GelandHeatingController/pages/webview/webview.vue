<template>
  <view>
    <web-view :webview-styles="webviewStyles" :src="navUrl"></web-view>
  </view>
</template>

<script>
export default {
  data() {
    return {
      webviewStyles: {
        progress: {
          color: "#FF3333",
        },
      },
      navUrl: "",
    };
  },
  onLoad(params) {
    this.navUrl = params.url;
  },
  methods: {},
};
</script>

<style></style>
