<template>
  	<div class="d-flex ma-4 justify-center">
    	<div class="middle-box" >
      		<board type="brown-board">
				<piece 
					v-for="item in mappedPieces"
					:key="`${item.position.x}${item.position.y}`"
					:type="item.type"
					:position="item.position" 
					/>
			</board>
		</div>
		<div class="right-box d-flex flex-column ml-4">
			<div class="flex-grow-1 mb-2">
                <controller />
            </div>
			<div class="vis flex-grow-1 mt-2">
                <logger />
            </div>
		</div>
  	</div>
</template>


<script>
import { mapState } from 'vuex'

export default {
  	name: "IndexPage",
  	computed: {
		mappedPieces() {
			let result = []
			let i = 0
			this.board.forEach(row => {
				let j = 0
				row.forEach(piece => {
					if (piece != 0) {
						result.push({type: piece, position: { x: j, y: i }})
					}
					j++
				})
				i++
			})
			console.log(result)
			return result
		},
    	...mapState(['board'])
  	},
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

<style scoped lang="scss">
.middle-box {
	width: 800px;
	height: 800px;
}

.right-box {
	width: 30%;
}
</style>