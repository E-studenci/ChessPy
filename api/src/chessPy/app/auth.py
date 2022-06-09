import flask_jwt_extended as jwt
import functools as fun
import datetime as dt
import flask

import chessPy.app.response_parser as rp
import chessPy.utils.environment as env
import chessPy.utils.errors as errors
from chessPy import JWT, APP, REDIS


@APP.after_request
def refresh_expiring_jwts(response: flask.Response):
    try:
        access_token = jwt.get_jwt()
        now = dt.datetime.now(dt.timezone.utc)
        target_timestamp = dt.datetime.timestamp(now + env.ENV.JWT_RENEW_TOKEN)
        if target_timestamp > access_token["exp"]:
            access_token = jwt.create_access_token(identity=jwt.get_jwt_identity())
            jwt.set_access_cookies(response, access_token)
        return response
    except (RuntimeError, KeyError):
        return response


def restricted_access(roles=[]):
    def wrapper(func):
        @fun.wraps(func)
        def decorator(*args, **kwargs):
            jwt.verify_jwt_in_request()
            if roles:
                claims = jwt.get_jwt()
                
                has_role = False
                for role in roles:
                    if role in claims["roles"]:
                        has_role = True
                
                if has_role:
                    return func(*args, **kwargs)
                else:
                    raise errors.UnauthorizedError(
                        name = "Unauthorized",
                        description = "Insufficient roles for this endpoint."
                    )
            return func(*args, **kwargs)
        return decorator
    return wrapper


@JWT.user_identity_loader
def user_identity_loader(user):
    return user


@JWT.user_lookup_loader
def user_lookup_loader(_jwt_header, jwt_data):
    identity = jwt_data["sub"]
    return identity


@JWT.token_in_blocklist_loader
def check_if_token_is_revoked(jwt_header, jwt_payload):
    jti = jwt_payload["jti"]
    token_in_redis = REDIS.get(jti)
    return token_in_redis is not None
    
    
@JWT.token_verification_failed_loader
def token_verification_failed_loader(_jwt_header, jwt_data):
    my_error = rp.ResponseError(
        name="Unauthorized",
        description="Invalid JWT"
    )
    return rp.ResponseData(error = my_error, status = 401)
    

@JWT.invalid_token_loader
def invalid_token_loader(msg):
    my_error = rp.ResponseError(
        name="Unauthorized",
        description=msg
    )
    return rp.ResponseData(error = my_error, status = 401)


@JWT.unauthorized_loader
def unauthorized_loader(msg):
    my_error = rp.ResponseError(
        name="Unauthorized",
        description=msg
    )
    return rp.ResponseData(error = my_error, status = 401)


@JWT.expired_token_loader
def expired_token_loader(_jwt_header, jwt_data):
    my_error = rp.ResponseError(
        name="Unauthorized",
        description="Token has expired"
    )
    return rp.ResponseData(error = my_error, status = 401)
