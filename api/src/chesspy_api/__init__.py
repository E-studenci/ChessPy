import flask_jwt_extended as jwt
import flask_socketio as fs
import flask_cors
import logging
import redis
import flask

import chesspy_api.utils.environment as config
import chesspy_api.database as database
import chesspy_api.app.roomhub as room_hub

logging.basicConfig(
    level=config.ENV.LOGGING_LEVEL,
    datefmt="%Y-%m-%d %H:%M:%S",
    format='{"time": "%(asctime)s", "level": "%(levelname)s", "message": "%(message)s"}',
)
LOGGER = logging.getLogger("werkzeug")
LOGGER.setLevel(level=config.ENV.LOGGING_LEVEL)

APP = flask.Flask("chesspy_api")

APP.config.from_object(config.ENV)

JWT = jwt.JWTManager(APP)

REDIS = redis.Redis(
    host=config.ENV.REDIS_HOST,
    port=config.ENV.REDIS_PORT,
    db=config.ENV.REDIS_DB,
    password=config.ENV.REDIS_PASS,
    decode_responses=True,
)

DB = database.Database()

CORS = flask_cors.CORS(APP, origins=config.ENV.ORIGINS, supports_credentials=True)

ROOM_HUB = room_hub.RoomHub(APP, LOGGER)

import chesspy_api.initialize_modules


def run_app(host: str = "127.0.0.1", port: int = 5000):
    # APP.run(host=host, port=port, use_reloader=False, debug=True)
    ROOM_HUB.socket.run(APP, host=host, port=port, use_reloader=False, debug=True)
