<template>
    <div class="d-flex align-center justify-center ma-2">
        <div class="main">
            <div class="d-flex justify-center mt-3">
                <v-btn class="bt mx-3" v-on:click="play1()">Countries</v-btn>
                <v-btn class="bt mx-3" v-on:click="play2()">Friend</v-btn>
                <v-btn class="bt mx-3" v-on:click="play3()">Jesica</v-btn>
            </div>
        </div>
    </div>
</template>


<script>
export default {
    name: "IndexPage",
    data() {
        return {
        }
    },
    computed: {
    },
    mounted() {
        // console.log(this.$sounds.countries)
    },
    methods: {
        play1() {
            if (!this.$sounds.countries.isPlaying.value) {
                this.$sounds.countries.play()
            } else {
                this.$sounds.countries.stop()
            }
        },
        play2() {
            if (!this.$sounds.friend.isPlaying.value) {
                this.$sounds.friend.play()
            } else {
                this.$sounds.friend.stop()
            }
        },
        play3() {
            if (!this.$sounds.jesica.isPlaying.value) {
                this.$sounds.jesica.play()
            } else {
                this.$sounds.jesica.stop()
            }
        }
    }
};
</script>

<style scoped lang="scss">
.main {
    background-color: rgb(49, 49, 49);
    border-radius: 1rem;
    width: 700px;
    height: 300px;
}

.bt {
    width: 30%;
}
</style>