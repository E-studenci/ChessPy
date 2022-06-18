# import chesspy

# board = chesspy.chess.Board(
#     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
# )

# print(chesspy.evaluateMove())

# print(board.fen)
# print(board.check)
# print(board.fifty_move_rule_draw)
# print(board.three_fold_repetition)

# alg = chesspy.Algorithms()

# print(f"Perft 5: {alg.perft(board, 5)}")
# print(f"Number of possible moves: {len(board.get_all_legal_moves())}")

# print(board)
# for _ in range(50):
#     result = alg.root(board, 99, 3000, True, True)
#     board.make_move(result.best_move)
#     print(board)
#     print(
#         f"Score: {result.score_after_best_move} | Max depth: {result.reached_depth} | Nodes {alg.count}")


def evaluate_move(board, move):
    return 1


import chesspy
import exposed

exposed.set_func(evaluate_move)

board = chesspy.Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

se = chesspy.SearchEngine(1)

result = se.root(board, 99, 1000)

print(result.reached_depth)