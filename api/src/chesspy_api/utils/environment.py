from os import getenv
import datetime as dt
import dotenv

dotenv.load_dotenv()


class Environment:
    DB_HOST = "127.0.0.1"
    DB_PORT = 5432
    DB_PROTOCOL = "postgresql+psycopg2"
    DB_USER = "root"
    DB_PASS = "password"
    DB_POOL_SIZE = 20
    REDIS_HOST = "127.0.0.1"
    REDIS_PORT = 6379
    REDIS_DB = "0"
    REDIS_PASS = "redis"
    REDIS_CRT = "redis.crt"
    CLIENT_KEY = "client.key"
    CLIENT_CRT = "client.crt"
    KEY_FOLDER = "tls"
    JWT_SECRET_KEY = "SECRET_KEY"
    JWT_COOKIE_SECURE = False
    JWT_RENEW_TOKEN_MINUTES = 7
    JWT_ACCESS_TOKEN_EXPIRES_MINUTES = 10
    JWT_TOKEN_LOCATION = ["cookies"]
    LOGGING_LEVEL = 20
    SECRET_KEY = "secret"

    @property
    def DB_URI(self) -> str:
        return f"{self.DB_PROTOCOL}://{self.DB_USER}:{self.DB_PASS}@{self.DB_HOST}:{self.DB_PORT}"

    @property
    def JWT_RENEW_TOKEN(self) -> dt.timedelta:
        return dt.timedelta(minutes=self.JWT_RENEW_TOKEN_MINUTES)

    @property
    def JWT_ACCESS_TOKEN_EXPIRES(self) -> dt.timedelta:
        return dt.timedelta(minutes=self.JWT_ACCESS_TOKEN_EXPIRES_MINUTES)

    def __init__(self) -> None:
        self._get_all_static_fields()

    @classmethod
    def _get_all_static_fields(cls) -> None:
        for name in cls.__dict__:
            if "__" not in name:
                new = getenv(name, None)
                if new:
                    old = getattr(cls, name)
                    setattr(cls, name, type(old)(new))


ENV = Environment()
