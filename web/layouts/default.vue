<template>
    <v-app dark>
        <v-app-bar fixed app>
            <div v-for="(item, i) in items" :key="i">
                <NuxtLink class="link" :to="item.to" v-text="item.title" />
            </div>
        </v-app-bar>

        <v-main>
            <Nuxt />
        </v-main>
        <v-footer absolute app>
            <span>&copy; {{ new Date().getFullYear() }}</span>
        </v-footer>
    </v-app>
</template>

<script>
export default {
    name: "DefaultLayout",
    data() {
        return {
            items: [
                {
                    title: "Home",
                    to: "/",
                },
                {
                    title: "Game",
                    to: "/game",
                },
                {
                    title: "History",
                    to: "/history",
                },
                {
                    title: "Settings",
                    to: "/settings",
                },
            ],
            miniVariant: false,
            title: "Vuetify.js",
        };
    },
};
</script>

<style scoped lang="scss">
.link {
    font-weight: 700;
    font-size: 1.5rem;
    margin-right: 1rem;
    text-decoration: none;
    color: white;
}

.link:hover {
    color: gray;
}
</style>