import flask
import chesspy

import chesspy_api.app.response_parser as rp
import chesspy_api.app.auth as auth
from chesspy_api import APP

anal_route = "/analysis"

alg = chesspy.SearchEngine(
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

@APP.route(f"{anal_route}", methods=["GET"])
@rp.response_wrapper()
def eval():
    fen = flask.request.args.get(
        "fen", default="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    )
    board = chesspy.Board(fen)
    time = flask.request.args.get("time", default=1000, type=int)
    if time > 4000:
        time = 4000
    elif time < 100:
        time = 1000

    result = alg.root(board, 99, time, True, True)
    return rp.ResponseData(
        payload={
            "eval": result.evaluation,
            "best_move": str(result.best_move),
            "best_move_opponent": str(result.best_opponent_move),
            "depth": result.reached_depth,
        }
    )
