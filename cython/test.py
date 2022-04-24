import chess

board = chess.Board(
    b"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

print(board)

cords1 = chess.Coordinates(0, 0)
cords2 = chess.Coordinates(4, 4)

# print(cords1)
# print(cords2)

move1 = chess.Move(cords1, cords2, 0)

print(move1)

# move1.destination = chess.Coordinates(5, 5)

# print(move1.destination)

board.make_move(move1)

print(board)
