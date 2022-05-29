import chess

board = chess.Board(
    b"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

alg = chess.Algorithms()

print(f"Perft 5: {alg.perft(board, 5)}")
print(f"Number of possible moves: {len(board.get_all_legal_moves())}")

print(board)
for _ in range(5):
    move, score, max_depth = alg.root(board, 10, 5000)
    board.make_move(move)
    print(board)
    print(f"Score: {score} | Max depth: {max_depth}")
