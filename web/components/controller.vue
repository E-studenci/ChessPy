<template>
    <div class="main d-flex flex-column">
        <div class="player-box d-flex align-center py-1">
            <div class="player-text ma-2">PLAYER 1</div>
        </div>
        <div
            class="
                flex-grow-1
                d-flex
                align-center
                justify-space-between
                middle
                py-1
            "
        >
            <v-btn class="but ml-2"><v-icon>mdi-pause</v-icon></v-btn>
            <v-btn class="but"><v-icon>mdi-ab-testing</v-icon></v-btn>
            <v-btn class="but mr-2"><v-icon>mdi-flag</v-icon></v-btn>
        </div>
        <div class="player-box d-flex align-center py-1">
            <div class="player-text ma-2">PLAYER 2</div>
        </div>
    </div>
</template>

<script>
// import { mapState, mapMutations } from 'vuex'
export default {
    props: {},
    data() {
        return {};
    },
    computed: {},
    methods: {},
    watch: {},
    mounted() {},
    beforeUnmount() {},
};
</script>

<style scoped lang="scss">
.main {
    background-color: rgb(49, 49, 49);
    height: 100%;
    border-radius: 1rem;
}

.player-box {
    flex: 0 0 30%;
}

.middle {
    background-color: rgb(65, 65, 65);
}

.but {
    width: 30%;
}

.player-text {
    font-size: 1.5rem;
}
</style>