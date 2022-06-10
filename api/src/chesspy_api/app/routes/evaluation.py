import chesspy_api.app.response_parser as rp
import chesspy_api.app.auth as auth
from chesspy_api import APP

eval_route = "/evaluation"
@APP.route(f"{eval_route}/<fen>", methods=["GET"])
@rp.response_wrapper()
def eval(fen:str):
    return rp.ResponseData(
        response = 'OK'
    )