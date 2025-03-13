import App from "./App";

import { createSSRApp } from "vue";
import store from "./store";
import EventBus from "./utils/eventBus";
import btManager from "./bluetooth/btManger";
import { addGlobalMessage } from "./utils/utils";

export function createApp() {
  const app = createSSRApp(App);
  //plugin
  app.use(store);

  // global properties
  uni.$eventHub = new EventBus();
  uni.$gBtManager = new btManager();
  uni.$globalMessageQueue = [];
  uni.addGlobalMessage = addGlobalMessage;
  return {
    app,
  };
}
