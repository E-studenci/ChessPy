<template>
  <div class="d-flex ma-4 justify-center">
    <div class="brown-board">
      <piece :type="3" />
    </div>
    <div class="right-box d-flex flex-column ml-4">
      <div class="vis flex-grow-1 mb-2"></div>
      <div class="vis flex-grow-1 mt-2"></div>
    </div>
    <!-- <button @click="click1()">ESS</button> -->
  </div>
</template>

<script>
export default {
  name: "IndexPage",
  mounted() {
    // this.socket = this.$nuxtSocket({
    //   // nuxt-socket-io opts:
    //   name: "main", // Use socket "home"
    //   channel: "/", // connect to '/index'

    //   // socket.io-client opts:
    //   reconnection: false,
    // });
    // this.socket.on("connect", () => {
    //   console.log("Connected");
    // });
    // this.socket.on("find_game", (msg, cb) => {
    //   console.log(msg);
    //   console.log(cb);
    // });
    // this.socket.on("game_started", (msg, cb) => {
    //   console.log(msg);
    //   console.log(cb);
    // });
    // this.socket.on("make_move_error", (msg, cb) => {
    //   console.log(msg);
    //   console.log(cb);
    // });
    // this.socket.on("game_ended", (msg, cb) => {
    //   console.log(msg);
    //   console.log(cb);
    // });
    // this.socket.on("turn_ended", (msg, cb) => {
    //   console.log(msg);
    //   console.log(cb);
    // });
    // this.socket.on("timers", (msg, cb) => {
    //   console.log(msg);
    //   console.log(cb);
    // });
  },
  methods: {
    // async click1() {
    //   this.socket.emit("find_game", { against_bot: true }, (resp) => {
    //     console.log(resp);
    //   });
    // },
  },
};
</script>

<style lang="scss">
.right-box {
  width: 30%;
}

.vis {
  background-color: rgb(51, 51, 26);
}
</style>