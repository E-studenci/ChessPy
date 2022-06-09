import chessPy.app.response_parser as rp
import chessPy.app.auth as auth
from chessPy import APP


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
