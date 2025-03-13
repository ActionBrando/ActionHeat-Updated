import { hex2ArrayBuffer, checkOS } from "../utils/utils.js";

const serviceId = "0000fff0-0000-1000-8000-00805f9b34fb";
const characteristicId_w = "0783b03e-8535-b5a0-7140-a304d2495cb8";
const characteristicId_r = "0783b03e-8535-b5a0-7140-a304d2495cba";

let timeoutHandler = null;

class btManager {
  constructor() {
    this.onBluetoothAdapterStateListener = {};
    this.onBluetoothDeviceFoundListener = {};
    this.onBLEConnectionStateChangeListener = {};
    this.onBLECharacteristicValueChangeListener = {};
    this.onNotifySuccessListener = {};
  }

  initManager() {
    //初始化监听器
    this.initListener();
    this.openBluetoothAdapter();
  }

  initListener() {
    //适配器监听
    uni.onBluetoothAdapterStateChange((res) => {
      // console.log("适配器状态变化", res);
      for (let key in this.onBluetoothAdapterStateListener) {
        this.onBluetoothAdapterStateListener[key](res);
      }
    });
    //发现设备监听
    uni.onBluetoothDeviceFound((res) => {
      // console.log("发现了设备", res);
      for (let key in this.onBluetoothDeviceFoundListener) {
        this.onBluetoothDeviceFoundListener[key](res);
      }
    });
    //监听设备连接监听
    uni.onBLEConnectionStateChange((res) => {
      // console.log("设备连接状态变化", res);
      for (let key in this.onBLEConnectionStateChangeListener) {
        this.onBLEConnectionStateChangeListener[key](res);
      }
      if (timeoutHandler) {
        clearTimeout(timeoutHandler);
        timeoutHandler = null;
      }
      uni.hideLoading();
      if (res.connected) {
        uni.showLoading({
          title: "Connected",
          mask: true,
        });
        //连接成功
        setTimeout(() => {
          this.notify(res.deviceId);
        }, 1500);
      }
    });
    //监听特征数据
    uni.onBLECharacteristicValueChange((res) => {
      // console.log("特征数据变化", res);
      for (let key in this.onBLECharacteristicValueChangeListener) {
        this.onBLECharacteristicValueChangeListener[key](res);
      }
    });
  }

  //添加适配器监听
  addBluetoothAdapterStateListener(key, listener) {
    this.onBluetoothAdapterStateListener[key] = listener;
  }

  //移除适配器监听
  removeBluetoothAdapterStateListener(key) {
    delete this.onBluetoothAdapterStateListener[key];
  }

  //添加发现设备监听
  addBluetoothDeviceFoundListener(key, listener) {
    this.onBluetoothDeviceFoundListener[key] = listener;
  }

  //移除发现设备监听
  removeBluetoothDeviceFoundListener(key) {
    delete this.onBluetoothDeviceFoundListener[key];
  }

  //添加设备连接监听
  addBLEConnectionStateChangeListener(key, listener) {
    this.onBLEConnectionStateChangeListener[key] = listener;
  }

  //移除设备连接监听
  removeBLEConnectionStateChangeListener(key) {
    delete this.onBLEConnectionStateChangeListener[key];
  }

  //添加特征数据监听
  addBLECharacteristicValueChangeListener(key, listener) {
    this.onBLECharacteristicValueChangeListener[key] = listener;
  }

  //移除特征数据监听
  removeBLECharacteristicValueChangeListener(key) {
    delete this.onBLECharacteristicValueChangeListener[key];
  }

  //添加订阅成功监听
  addNotifySuccessListener(key, listener) {
    this.onNotifySuccessListener[key] = listener;
  }

  //移除订阅成功监听
  removeNotifySuccessListener(key) {
    delete this.onNotifySuccessListener[key];
  }

  //打开蓝牙适配器
  openBluetoothAdapter() {
    uni.openBluetoothAdapter({
      success: (res) => {},
      fail: (err) => {
        console.log("openBluetoothAdapter fail", err);
      },
    });
  }

  //关闭蓝牙适配器
  closeBluetoothAdapter() {
    return new Promise((resolve, reject) => {
      uni.closeBluetoothAdapter({
        success: (res) => {
          resolve(res);
        },
        fail: (err) => {
          reject(err);
        },
      });
    });
  }

  //启动扫描
  startScan() {
    uni.startBluetoothDevicesDiscovery({
      allowDuplicatesKey: checkOS() == "iOS",
      success: (res) => {},
    });
    //持续20秒后关闭
    timeoutHandler = setTimeout(() => {
      this.stopScan();
    }, 20000);
  }

  //停止扫描
  stopScan() {
    uni.stopBluetoothDevicesDiscovery((res) => {
      console.log(res);
    });
    if (timeoutHandler) {
      clearTimeout(timeoutHandler);
      timeoutHandler = null;
    }
  }

  //连接设备
  toConnect(deviceId) {
    //停止扫描
    uni.stopBluetoothDevicesDiscovery((res) => {});
    //开始连接
    uni.createBLEConnection({
      deviceId: deviceId,
      timeout: 20000,
      success(res) {},
    });
    uni.showLoading({
      title: "Connecting...",
      mask: true,
    });
    timeoutHandler = setTimeout(() => {
      uni.hideLoading();
    }, 10000);
  }

  //断开连接
  toDisconnect(deviceId) {
    uni.showLoading({
      title: "Disconnecting...",
      mask: true,
    });
    uni.closeBLEConnection({ deviceId: deviceId });
    timeoutHandler = setTimeout(() => {
      uni.hideLoading();
    }, 10000);
  }

  //刷新服务和订阅特征
  notify(deviceId) {
    //订阅特征值
    let that = this;
    uni.notifyBLECharacteristicValueChange({
      deviceId: deviceId,
      serviceId: serviceId,
      characteristicId: characteristicId_r,
      success(res) {
        uni.hideLoading();
        for (let key in that.onNotifySuccessListener) {
          that.onNotifySuccessListener[key](deviceId);
        }
      },
      fail(res1) {
        console.log(res1);
      },
    });
  }

  writeData(deviceId, data, callback) {
    let convertData = hex2ArrayBuffer(data);
    setTimeout(() => {
      uni.writeBLECharacteristicValue({
        deviceId: deviceId,
        serviceId: serviceId,
        characteristicId: characteristicId_w,
        value: convertData,
        success(res) {
          if (callback) {
            callback();
          }
        },
        fail(res1) {
          console.log("fail", res1);
        },
      });
    }, 200);
  }

  writeDataSync(deviceId, data) {
    let convertData = hex2ArrayBuffer(data);
    return new Promise((resolve, reject) => {
      uni.writeBLECharacteristicValue({
        deviceId: deviceId,
        serviceId: serviceId,
        characteristicId: characteristicId_w,
        value: convertData,
        success(res) {
          resolve(res);
        },
        fail(res1) {
          reject(res1);
        },
      });
    });
  }
}

export default btManager;
