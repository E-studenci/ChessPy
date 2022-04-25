import chess

board = chess.Board(
    b"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

print(board)

cords1 = chess.Coordinates(4, 4)
cords2 = chess.Coordinates(4, 3)

print(cords1)
print(cords2)

move1 = chess.Move(cords1, cords2, 0)

# board.make_move(move1)

print(board)

legal_moves = board.get_all_legal_moves()

# print(legal_moves)

alg = chess.Algorithms()

amount = alg.perft_starter_single_thread(board, 3)

print(amount)

# print(alg_board.get_all_legal_moves())
