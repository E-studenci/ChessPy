<template>
  <div>
    Siema
  </div>
</template>

<script>
export default {
  name: 'IndexPage',
  methods: {
    async fetchData() {
      const config = await this.$axios.$get('/config.json')
      console.log(config)
    }
  },
  mounted() {
    this.fetchData()
  }
}
</script>
