import chess

board = chess.Board("rnb1kbnr/1p1p1ppp/pKp5/4p3/4P3/1q4P1/PPPP3P/RNB1QBNR w kq - 6 20")

print(board.fen)
print(board.check)
print(board.fifty_move_rule_draw)
print(board.three_fold_repetition)

alg = chess.Algorithms()

print(f"Perft 5: {alg.perft(board, 5)}")
print(f"Number of possible moves: {len(board.get_all_legal_moves())}")

print(board)
for _ in range(10):
    move, score, max_depth = alg.root(board, 99, 500)
    board.make_move(move)
    print(board)
    print(f"Score: {score} | Max depth: {max_depth} | Nodes {alg.count}")
