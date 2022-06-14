<template>
    <div :class="`${piece} ${images[type]}`" />
</template>

<script>
export default {
    components: true,
    props: {
        type: {
            type: Number,
            required: true
        }
    },
    data() {
        return {
            images: {
                12: "black-pawn",
                6: "white-pawn",
                10: "black-bishop",
                4: "white-bishop",
                9: "black-knight",
                3: "white-knight",
                11: "black-rook",
                5: "white-rook",
                8: "black-queen",
                2: "white-queen",
                7: "black-king",
                1: "white-king",
            }
        }
    }
}
</script>