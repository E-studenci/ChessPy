<template>
    <div class="main d-flex flex-column">
        <div class="chat"></div>
        <div class="d-flex align-center justify-center">
            <v-text-field
                class="pa-3"
                v-model="message"
                label="Chat"
                outlined
                clearable
            />
            <v-btn class="mb-8 mr-2">Send</v-btn>
        </div>
    </div>
</template>

<script>
// import { mapState, mapMutations } from 'vuex'
export default {
    props: {},
    data() {
        return {
            message: "",
        };
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

.chat {
    flex: 0 0 85%;
}
</style>