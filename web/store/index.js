
export const state = () => ({
    clipped: null,
    mousePosition: { x: 0, y: 0 },
    board: [
        [0, 0, 0, 0, 0, 0, 0, 0],
        [12, 12, 12, 12, 12, 12, 12, 12],
        [0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0],
        [6, 6, 6, 6, 6, 6, 6, 6],
        [0, 0, 0, 0, 0, 0, 0, 0],
    ],
    boardRect: null
})


export const mutations = {
    setClipped(state, piece) {
        state.clipped = piece
    },
    setMousePosition(state, mousePosition) {
        state.mousePosition = mousePosition
    },
    setBoard(state, board) {
        state.board = board
    },
    setBoardRect(state, boardRect) {
        state.boardRect = boardRect
    },
}
