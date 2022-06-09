import chessPy.app.response_parser as rp
import chessPy.app.auth as auth
from chessPy import APP

eval_route = "/evaluation"
@APP.route(f"{eval_route}/<fen>", methods=["GET"])
@rp.response_wrapper()
def eval(fen:str):
    return rp.ResponseData(
        response = 'OK'
    )