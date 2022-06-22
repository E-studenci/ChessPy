from sqlalchemy.orm import declarative_base, relationship
from sqlalchemy.ext.declarative import DeclarativeMeta
from dataclasses import dataclass
from datetime import datetime
import sqlalchemy as db
import bcrypt
import enum
import json

TABLE_GAMES = "GAMES".lower()
TABLE_USERS = "USERS".lower()
TABLE_ROLES = "ROLES".lower()
TABLE_USER_ROLES = "USER_ROLES".lower()

TABLE_GAMES = "TABLE_GAMES".lower()
TABLE_GAME_HISTORY = "TABLE_GAME_HISTORY".lower()


Base = declarative_base()

# @dataclass
# class Game(Base):
#     __tablename__ = TABLE_GAMES

#     id = db.Column(db.Integer, primary_key=True, autoincrement=True)
#     login = db.Column(db.String(30), unique=True, index=True)
#     password = db.Column(db.String(255))
#     name = db.Column(db.String(30))
#     surname = db.Column(db.String(30))

#     roles = relationship('Role', secondary=TABLE_USER_ROLES, lazy='noload', back_populates="users")
#     client_account = relationship("ClientAccount", back_populates="user", uselist=False, lazy='noload')
#     manager_account = relationship("ManagerAccount", back_populates="user", uselist=False, lazy='noload') TODO: Store games


@dataclass
class User(Base):
    __tablename__ = TABLE_USERS

    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    login = db.Column(db.String(30), unique=True, index=True)
    password = db.Column(db.String(255))
    ranking = db.Column(db.Integer)

    roles = relationship(
        "Role", secondary=TABLE_USER_ROLES, lazy="noload", back_populates="users"
    )

    def verify_password(self, password):
        return bcrypt.checkpw(password.encode("utf8"), self.password.encode("utf8"))

    def roles_to_str(self):
        ret_list = []
        if self.roles:
            for role in self.roles:
                ret_list.append(role.name)
        return ret_list


@dataclass
class Role(Base):
    __tablename__ = TABLE_ROLES

    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    name = db.Column(db.String(10))

    users = relationship(
        "User", secondary=TABLE_USER_ROLES, lazy="noload", back_populates="roles"
    )


user_roles = db.Table(
    TABLE_USER_ROLES,
    Base.metadata,
    db.Column(
        "user_id", db.ForeignKey(f"{TABLE_USERS}.id"), index=True, primary_key=True
    ),
    db.Column(
        "role_id", db.ForeignKey(f"{TABLE_ROLES}.id"), index=True, primary_key=True
    ),
)


class Game(Base):
    __tablename__ = TABLE_GAMES

    id = db.Column(db.Integer, primary_key=True, autoincrement=True)

    player_white_id = db.Column(db.Integer, db.ForeignKey(TABLE_USERS + ".id"))
    player_black_id = db.Column(db.Integer, db.ForeignKey(TABLE_USERS + ".id"))


class GameHistory(Base):
    __tablename__ = TABLE_GAME_HISTORY

    geme_id = db.Column(
        db.Integer, db.ForeignKey(TABLE_GAMES + ".id"), primary_key=True
    )
    index = db.Column(db.Integer, primary_key=True)
    fen = db.Column(db.String)
