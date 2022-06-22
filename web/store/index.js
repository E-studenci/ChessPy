
export const state = () => ({
    clipped: null,
    mousePosition: { x: 0, y: 0 },
    board: [
        [11, 9, 10, 8, 7, 10, 9, 11],
        [12, 12, 12, 12, 12, 12, 12, 12],
        [0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0],
        [6, 6, 6, 6, 6, 6, 6, 6],
        [5, 3, 4, 2, 1, 4, 3, 5],
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
