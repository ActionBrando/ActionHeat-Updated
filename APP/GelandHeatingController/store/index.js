import bluetooth from "./modules/bluetooth";
import { createStore } from "vuex";
const store = createStore({
  modules: {
    bluetooth,
  },
});
export default store;
