import sqlalchemy.exc as sqlalchemy_exc
import sqlalchemy.orm as orm
import functools as fun
import typing as t
import sqlalchemy
import logging

import chessPy.utils.errors as errors
import chessPy.utils.environment as config

class Database:
    def __init__(self) -> None:
        self.engine = sqlalchemy.create_engine(config.ENV.DB_URI)
    
    def db_query(self, fix_tuple=False):
        def wrapper(func):
            @fun.wraps(func)
            def decorator(*args, **kwargs) -> t.Any:
                try:
                    with self.engine.connect() as connection:
                        with orm.Session(bind=connection) as session:
                            result = func(session, *args , **kwargs)
                        if fix_tuple and result:
                            return result[0]
                        return result
                except sqlalchemy_exc.IntegrityError as int_error:
                    raise errors.DatabaseError(
                        name = "Integrity Error",
                        description = int_error.detail
                    )
                except sqlalchemy_exc.SQLAlchemyError as db_error:
                    logging.error(db_error)
                    raise errors.DatabaseError(
                        name = "Connection Error",
                        description = str(db_error)
                    )
                except Exception as error:
                    logging.error(error)
                    raise errors._GenericError(
                        name = "Unknown error",
                        description = ""
                    )
            return decorator
        return wrapper
