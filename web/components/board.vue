<template>
    <div :class="type" ref="chessboard" draggable="false" >
        <slot>
        </slot>
    </div>
</template>

<script>
import { mapState, mapMutations } from 'vuex'


export default {
    props: {
        type: {
            type: String,
            required: true
        }
    },
    data() {
        return {}
    },
    computed: {
        ...mapState(['mousePosition'])
    },  
    methods: {
        ...mapMutations({ setMousePosition: 'setMousePosition', setBoardRect: 'setBoardRect' })
    },
    watch: {
    },
    mounted() {
        let that = this
        that.setBoardRect(
            that.$refs.chessboard?.getBoundingClientRect()
        )
        that.$refs.chessboard.onmousemove = function(e) {
            let boundingRect = that.$refs.chessboard.getBoundingClientRect()
            that.setBoardRect(boundingRect)
            let x = e.clientX - boundingRect.left
            let y = e.clientY - boundingRect.top
            that.setMousePosition({ x: x, y: y })
        }
    },
    beforeUnmount() {
        that.$refs.chessboard.onmousemove = null
    }
}
</script>