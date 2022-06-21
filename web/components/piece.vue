<template>
    <div 
        :class="`piece ${pieceType} ${this.isClipped ? 'dragging' : ''}`"
        :style="style" 
        @mousedown="clip()"
        @mouseup="unclip()"
        ref="piece" 
        draggable="false" />
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
        position: {
            type: Object,
            required: true
        }
    },
    data() {
        return {
            pieceType: pieceCssMapping[this.type],
            isClipped: false
        }
    },
    computed: {
        x() {
            if (this.boardRect) {
                if (this.isClipped) {
                    return this.mousePosition.x - (this.boardRect.width / 16)
                }
                return this.position.x * (this.boardRect.width / 8)
            }
            return 0
        },
        y() {
            if (this.boardRect) {
                if (this.isClipped) {
                    return this.mousePosition.y - (this.boardRect.height / 16)
                }
                return this.position.y * (this.boardRect.width / 8)
            }
            return 0
        },
        style() {
            return {
                transform: `translate(${this.x}px, ${this.y}px)`
            }
        },
        ...mapState(['clipped', 'mousePosition', 'boardRect'])
    },
    methods: {
        getBoundingClientRect() {
            return this.$refs?.piece?.getBoundingClientRect()
        },
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
        ...mapMutations({ setClipped: 'setClipped' })
    }
}
</script>