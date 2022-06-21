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


# def evaluate_move(board, move):
    # return 1

import chesspy

board = chesspy.Board(
    "1k1r2nr/p1p1b2p/Bpnp2p1/4qp2/1P3PP1/PQP1P1PN/1B1PN3/1K1R3R b - - 0 1"
)

se = chesspy.SearchEngine(
    chesspy.MoveOrderingType.HANDCRAFTED,
    chesspy.SearchParams(
        use_null_move_pruning=False,
        use_killer_moves=True,
        use_hashed_positions=True,
        use_hashed_moves=True,
        use_quiescence=True,
        use_check_extension=True,
        use_MVVLVA=True,
    ),
)

result = se.root(board, 99, 2000)

print(result.best_move)
print(result.reached_depth)
print(result.node_count)