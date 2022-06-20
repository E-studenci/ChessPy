
export const state = () => ({
    clipped: null
})


export const mutations = {
    set(state, piece) {
        console.log(piece)
        state.clipped = piece
    }
}
