import chess

board = chess.Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

print(board.fen)
print(board.check)
print(board.fifty_move_rule_draw)
print(board.three_fold_repetition)

alg = chess.Algorithms()

print(f"Perft 5: {alg.perft(board, 5)}")
print(f"Number of possible moves: {len(board.get_all_legal_moves())}")

print(board)
for _ in range(10):
    result = alg.root(board, 99, 500, True, True)
    board.make_move(result.best_move)
    print(board)
    print(f"Score: {result.score_after_best_move} | Max depth: {result.reached_depth} | Nodes {alg.count}")
