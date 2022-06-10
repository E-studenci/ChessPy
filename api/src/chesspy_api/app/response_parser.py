import flask.wrappers as flask_wrappers
import functools as fun
import flask
import json

import chesspy_api.utils.json_validation.json_methods as json_methods
import chesspy_api.utils.errors as errors

RESPONSE_CODES = [
    (100, 199, 'info'),
    (200, 299, 'success'),
    (300, 399, 'redirect'),
    (400, 499, 'client-error'),
    (500, 599, 'server-error')
]

def parse_code(code: int) -> str:
    for tup in RESPONSE_CODES:
        if tup[0] <= code <= tup[1]:
            return tup[2]
    return 'unknown'


class ResponseError(dict):
    def __init__(self, description: str = '', name: str = '', error = None) -> None:
        if error is None:
            dict.__init__(self, description=description, name=name)
            self.description = description
            self.name = name
        else:
            dict.__init__(self, description=error.description, name=error.name)
            self.description = error.description
            self.name = error.name


class ResponseData(flask.Response):
    def __init__(self, payload = None, error = None, status = 200, headers = None, mimetype = None, content_type = None, direct_passthrough = False) -> None:
        super().__init__("", status, headers, mimetype, content_type, direct_passthrough)
        self.__all = {
            'data': None,
            'error': None,
            'status': parse_code(status)
        }
        self.payload = payload
        self.error = error
        self.code = status
        self.mimetype = 'application/json'
    
    @property
    def payload(self):
        return self.__all['data']

    @payload.setter
    def payload(self, var):
        self.__all["data"] = var
        self.data = json.dumps(self.__all)

    @property
    def error(self):
        return self.__all['error']

    @error.setter
    def error(self, var):
        self.__all["error"] = var
        self.data = json.dumps(self.__all)
    
    @property
    def code(self):
        return self.__all['code']
    
    @code.setter
    def code(self, var):
        self.__all["status"] = parse_code(var)
        self.data = json.dumps(self.__all)


def response_wrapper(json_schema=None):
    def wrapper(func):
        @fun.wraps(func)
        def decorator(*args, **kwargs):
            flask_request = flask.request
            if json_schema:
                if not flask_request.is_json:
                    raise errors.BadRequestError(
                        name='Mimetype error',
                        description='Provide application/json or application/*+json content type.'
                    )
                
                valid, error = json_methods.validate_json_structure(flask_request.get_data(as_text = True))
                if not valid:
                    raise errors.BadRequestError(
                        name='Invalid Json',
                        description=error
                    )
                
                valid, error = json_methods.validate_json_schema(json_schema, flask_request.json)
                if not valid:
                    raise errors.BadRequestError(
                        name='Invalid Json Schema',
                        description=error
                    )       
            return func(*args, **kwargs)
        return decorator
    return wrapper

