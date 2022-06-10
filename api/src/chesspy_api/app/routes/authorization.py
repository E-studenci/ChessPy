import flask_jwt_extended as jwt
import flask
import json

import chesspy_api.app.response_parser as rp
import chesspy_api.utils.environment as env
import chesspy_api.utils.errors as errors
import chesspy_api.app.auth as auth
from chesspy_api import APP


@APP.route("/login", methods=["GET"])
@rp.response_wrapper()
def login():
    auth = flask.request.authorization
    if not auth:
        raise errors.UnauthorizedError(
            name="Unauthorized",
            description="No 'Authorization' header supplied."
        )
    
    if auth.username == "" or auth.password == "":
        raise errors.UnauthorizedError(
            name="Unauthorized",
            description="Password or username is missing."
        )
    
    user = get_user(auth.username)
    
    if not user:
        raise errors.UnauthorizedError(
            name="Unauthorized",
            description="User with provided username does not exist."
        )

    if not user.password == auth.password: # TODO use user's method
        raise errors.UnauthorizedError(
            name="Unauthorized",
            description="Incorrect password."
        )

    access_token = jwt.create_access_token(
        identity=user.id, 
        additional_claims={
            "roles": user.roles
        }
    )
    
    response = rp.ResponseData()
    jwt.set_access_cookies(response, access_token)
    return response


@APP.route("/logout", methods=["POST"])
@auth.restricted_access()
@rp.response_wrapper()
def logout():
    response = rp.ResponseData()
    
    jwt.unset_jwt_cookies(response)
    return response



class User:
    def __init__(self, id, username, password, roles) -> None:
        self.id = id
        self.username = username
        self.password = password
        self.roles = roles

USERS = [
    User("348964984", "login1", "password1", ["admin"]),
    User("812738743", "login2", "password2", ["client"])
]

def get_user(username: str):
    for user in USERS:
        if username == user.username:
            return user