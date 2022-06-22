<template>
    <div class="d-flex flex-column align-right justify-center ma-2">
        <div class="element d-flex flex-grow-1 ma-1 pa-2">
            <div class="flex-grow-1">Me</div>
            <div class="flex-grow-1">Oponent</div>
            <div class="flex-grow-1">Outcome</div>
        </div>
        <div class="element d-flex flex-grow-1 ma-1 pa-2" v-for="(match, index) in data" :key="index">
            <div class="flex-grow-1">{{ match.me }}</div>
            <div class="flex-grow-1">{{ match.oponent }}</div>
            <div class="flex-grow-1">{{ match.outcome }}</div>
        </div>
    </div>
</template>


<script>
export default {
    name: "IndexPage",
    data() {
        return {
            data: [
                {
                    me: "Player 1",
                    oponent: "Player 2",
                    outcome: "Win"
                },
                {
                    me: "Player 1",
                    oponent: "Player 3",
                    outcome: "Lose"
                },
                {
                    me: "Player 1",
                    oponent: "Player 4",
                    outcome: "Win"
                },
                {
                    me: "Player 1",
                    oponent: "Player 5",
                    outcome: "Win"
                },
            ]
        }
    },
    computed: {
    },
    mounted() {
    },
    methods: {}
};
</script>

<style scoped lang="scss">
.element {
    background-color: rgb(49, 49, 49);
    border-radius: 1rem;
}

.bt {
    width: 30%;
}
</style>