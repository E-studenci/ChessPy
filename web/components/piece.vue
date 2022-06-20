<template>
    <div :class="`piece ${pieceType}`" :style="style" @mousedown="clip()" @mouseup="unclip()" />
</template>

<script>
import { pieceCssMapping } from '@/static/mappings'
import { mapState, mapMutations } from 'vuex'

export default {
    props: {
        type: {
            type: Number,
            required: true
        },
        mousePosition: {
            type: Number,
            required: false
        }
    },
    data() {
        return {
            pieceType: pieceCssMapping[this.type],
            isClipped: false
        }
    },
    computed: {
        style() {
            return {
                transform: `translate(${100}%, ${100}%)`
            }
        },
        ...mapState(['clipped'])
    },
    methods: {
        clip() {
            if (!this.clipped && !this.isClipped) {
                this.isClipped = true
                this.setClipped(this.type)

            }
        },
        unclip() {
            if (this.isClipped) {
                this.isClipped = false
                this.setClipped(null)
            }
        },
        ...mapMutations({ setClipped: 'set' })
    }
}
</script>