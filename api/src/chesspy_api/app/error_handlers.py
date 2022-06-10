import werkzeug.exceptions as exc
import flask


import chesspy_api.app.response_parser as rp
import chesspy_api.utils.errors as errors
from chesspy_api import APP


@APP.errorhandler(exc.HTTPException)
def handle_exception(error):
    my_error = rp.ResponseError(error = error)
    return rp.ResponseData(error = my_error, status = error.code)


@APP.errorhandler(errors.DatabaseError)
def database_error(error: errors.DatabaseError):
    my_error = rp.ResponseError(error = error)
    return rp.ResponseData(error = my_error, status = 500)


@APP.errorhandler(errors.UnauthorizedError)
def unauthorized_error(error: errors.UnauthorizedError):
    my_error = rp.ResponseError(error = error)
    return rp.ResponseData(error = my_error, status = 401)


@APP.errorhandler(errors.BadRequestError)
def unauthorized_error(error: errors.BadRequestError):
    my_error = rp.ResponseError(error = error)
    return rp.ResponseData(error = my_error, status = 400)


@APP.errorhandler(errors.DynamicCodeError)
def unknown_error(error: errors.DynamicCodeError):
    my_error = rp.ResponseError(error = error)
    return rp.ResponseData(error = error.code, status = error.code)