import chesspy_api.app.response_parser as rp
import chesspy_api.app.auth as auth
from chesspy_api import APP


@APP.route("/")
@rp.response_wrapper()
def index():
    return rp.ResponseData(
        response = 'OK'
    )

@APP.route("/admin")
@auth.restricted_access(["admin"])
@rp.response_wrapper()
def admin():
    return rp.ResponseData(
        response = 'OK'
    )
