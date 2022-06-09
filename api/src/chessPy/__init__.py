import flask_jwt_extended as jwt
import flask_cors
import logging
import flask

import chessPy.utils.environment as config
import chessPy.database.db as database
import chessPy.app.roomhub.room_hub as room_hub
logging.basicConfig(
    level=config.ENV.LOGGING_LEVEL, 
    datefmt="%Y-%m-%d %H:%M:%S",
    format='{"time": "%(asctime)s", "level": "%(levelname)s", "message": "%(message)s"}'
)
logging.getLogger('werkzeug').setLevel(
    level=config.ENV.LOGGING_LEVEL
)
    
APP = flask.Flask("chessPy")

APP.config.from_object(config.ENV)

JWT = jwt.JWTManager(APP)

DB = database.Database()

CORS = flask_cors.CORS(APP, supports_credentials = True)

ROOM_HUB = room_hub.RoomHub(APP)

import chessPy.initialize_modules

def run_app(host: str = '127.0.0.1', port: int = 5000):
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)
    APP.run(host=host, port=port, use_reloader=False, debug=True)