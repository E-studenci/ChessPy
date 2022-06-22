import json
import uuid
import enum
import random
import threading
from functools import partial
import flask
import chesspy
import flask_socketio as fs
from flask import request


class GameStatus(enum.Enum):
    ONGOING = 0
    MATE = 1
    STALEMATE = 2
    THREEFOLD_REPETITION = 4
    FIFTY_MOVER_RULE = 3
    INSUFFICIENT_MATERIAL = 5
    DRAW = 6
    TIME_RAN_OUT = 7
    DISCONNECTED = 8


class Player:
    def __init__(self, id, is_bot):
        self.id = id
        self.is_bot = is_bot


def move_from_dict(move):
    return chesspy.Move(
        chesspy.Coordinates(move["origin_row"], move["origin_column"]),
        chesspy.Coordinates(move["destination_row"], move["destination_column"]),
        move["promotion"],
    )


def move_to_dict(move):
    return {
        "origin_row": move.origin.row,
        "origin_column": move.origin.column,
        "destination_row": move.destination.row,
        "destination_column": move.destination.column,
        "promotion": move.promotion,
    }


def coordinates_to_dict(coordinates):
    return "row" + str(coordinates.row) + "column" + str(coordinates.column)

engine = chesspy.SearchEngine(
    chesspy.MoveOrderingType.HANDCRAFTED,
    chesspy.SearchParams(
        use_null_move_pruning=False,
        use_killer_moves=True,
        use_hashed_positions=True,
        use_hashed_moves=True,
        use_quiescence=True,
        use_check_extension=True,
        use_MVVLVA=True,
    ),
)


def bot_make_move(board, room_id):
    from chesspy_api import ROOM_HUB

    result = engine.root(board, 99, 1000)
    best_move = result.best_move
    ROOM_HUB.make_move(True, {"room_id": room_id, "move": move_to_dict(best_move)})


# SOCKET = None
class RoomHub:
    open_rooms: list["Room"] = {}
    rooms: dict[uuid.UUID, "Room"] = {}

    def __init__(
        self, flask_app: flask.Flask, logger, cors_allowed_origins="*"
    ) -> None:
        self.socket = fs.SocketIO(
            flask_app,
            logger=True,
            engineio_logger=True,
            cors_allowed_origins=cors_allowed_origins,
        )  # TODO: TEMP
        self.socket.on_event("find_game", self.find_game)
        self.socket.on_event("make_move", partial(self.make_move, is_bot=False))
        self.socket.on_event("disconnect", self.disconnected)

    def find_game(self, json_data):
        data = json_data
        if data["against_bot"]:
            game_room = Room(uuid.uuid1, Player(request.sid, False), self.socket)
            self.rooms[game_room.id] = game_room
            fs.join_room(game_room.id)
            self.socket.emit(
                "find_game",
                json.dumps({"found": True, "message": "Found an open bot!"}),
                to=game_room.id,
            )
            game_room.add_player(Player(-1, True))
            self.start_room(game_room.id)
        else:
            if len(self.open_rooms > 0):
                game_room = self.open_rooms.pop()
                fs.join_room(game_room.id)
                self.socket.emit(
                    "find_game",
                    json.dumps({"found": True, "message": "Found an opponent"}),
                    to=game_room.id,
                )
                game_room.add_player(Player(request.sid, False))
                self.rooms[game_room.id] = game_room
                self.start_room(game_room.id)
            else:
                game_room = Room(uuid.uuid1, Player(request.sid, False), self.socket)
                self.open_rooms.append(game_room)
                fs.join_room(game_room.id)
                self.socket.emit(
                    "find_game",
                    json.dumps({"found": False, "message": "Looking for an opponent"}),
                    to=request.sid,
                )

    def start_room(self, game_room_id):
        room = self.rooms[game_room_id]
        res = room.start_game()
        if not any(player.is_bot for player in res[0]):
            self.socket.emit(
                "game_started",
                json.dumps({"color": 0, "legal_moves": res[1]}),
                to=res[0][0],
            )
            self.socket.emit("game_started", json.dumps({"color": 1}), to=res[0][1])
        else:
            if res[0][0].is_bot:
                bot_make_move(room.board, game_room_id)
                self.socket.emit("game_started", json.dumps({"color": 1}), to=res[0][1])
            else:
                self.socket.emit(
                    "game_started",
                    json.dumps({"color": 0, "legal_moves": res[1]}),
                    to=res[0][0],
                )

    def make_move(self, is_bot, json_data):
        if not is_bot:
            data = json_data
            room_id = data["room_id"]
            if not self.rooms[room_id]:  # the room doesn't exist
                self.socket.emit(
                    "make_move_error",
                    {"message": "The game does not exist"},
                    to=request.sid,
                )
                return
            else:
                room = self.rooms[room_id]
                result = room.make_move_check(request.sid, move_from_dict(data["move"]))
                if not result[0]:  # the move wasn't legal
                    self.socket.emit(
                        "make_move_error", {"message": result[1]}, to=request.sid
                    )
                else:
                    room.make_move(move_from_dict(data["move"]))
                    res = room.end_turn()
                    if not res[0]:  # the game ended after the move
                        game_status = room.end_game()
                        self.socket.emit(
                            "game_ended", json.dumps(game_status), to=room_id
                        )
                        self.socket.close_room(room_id)
                    else:
                        self.socket.emit("turn_ended", json.dumps(res[1]), to=room_id)
                        if room.players[room.turn].is_bot:
                            bot_make_move(room.board, room.id)
        else:
            data = json_data
            room_id = data["room_id"]
            room = self.rooms[room_id]
            room.make_move(move_from_dict(data["move"]))
            res = room.end_turn()
            if not res[0]:  # the game ended after the move
                game_status = room.end_game()
                self.socket.emit("game_ended", json.dumps(game_status), to=room_id)
                self.socket.close_room(room_id)
            else:
                self.socket.emit("turn_ended", json.dumps(res[1]), to=room_id)
                if room.players[room.turn].is_bot:
                    bot_make_move(room.board, room.id)

    def disconnected(self):
        for i, room in enumerate(self.open_rooms):
            if request.sid in room.players:
                self.socket.close_room(room.id)
                self.open_rooms.remove(i)
        for room in self.rooms.values():
            if request.sid in room.players:
                room.end_game()
                self.socket.emit(
                    "game_ended",
                    json.dumps(
                        {
                            "status": GameStatus.DISCONNECTED,
                            "winner": 1 if room.players[0] == request.sid else 0,
                        }
                    ),
                    to=room.id,
                )
                self.socket.close_room(room.id)
                self.rooms.pop(room.id)


class RepeatTimer(threading.Timer):
    def run(self):
        while not self.finished.wait(self.interval):
            self.function(*self.args, **self.kwargs)


class Room:
    game_status: GameStatus
    socket = None
    timer = RepeatTimer(1, print)

    def __init__(
        self,
        id,
        player_one: Player,
        socket: fs.SocketIO,
        fen: str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    ):
        self.id = id
        self.players = [player_one]
        self.board = chesspy.Board(fen)
        self.socket = socket
        self.time_after_turn = 0

    def add_player(self, player_two: Player):
        self.players.append(player_two)

    def start_game(self):
        self.turn = 0
        random.shuffle(self.players)
        self.times = [600, 600]
        self.timer = RepeatTimer(1, self.timer_fun)
        all_legal_moves = self.board.get_all_legal_moves()
        self.timer.start()
        moves_dict = list(map(lambda move: move_to_dict(move), all_legal_moves))
        return (self.players, moves_dict)

    def make_move_check(self, player_id, chosen_move: chesspy.Move):
        if not any(player.id == player_id for player in self.players):
            return (False, "It's not your game")
        if self.players[self.turn] != player_id:
            return (False, "It's not your turn")
        if self.game_status != GameStatus.ONGOING:
            return (False, "The game is not active")
        if not any(chosen_move in moves for moves in self.board.get_all_legal_moves()):
            return (False, "No cheating!")

    def make_move(self, chosen_move):
        self.board.make_move(chosen_move)
        self.timer.cancel()
        self.times[self.turn] += self.time_after_turn

    def end_turn(self):
        self.game_status = GameStatus(self.board.game_status)
        self.turn = int(not self.turn)
        if self.game_status != GameStatus.ONGOING:
            return (False, self.board)
        else:
            self.timer = RepeatTimer(1, self.timer_fun)
            self.timer.start()
            all_legal_moves = self.board.get_all_legal_moves()
            return (
                True,
                {
                    "board": self.board.board,
                    "turn": self.turn,
                    "legal_moves": (
                        list(map(lambda move: move_to_dict(move), all_legal_moves))
                    ),
                    "king_in_check": self.board.check,
                },
            )

    def end_game(self):
        try:
            self.timer.cancel()
        except:
            pass
        if any(time <= 0 for time in self.times):
            self.game_status = GameStatus.TIME_RAN_OUT
            return {
                "status": self.game_status,
                "winner": 1 if self.timers[0] <= 0 else 0,
            }
        elif any(
            self.game_status == status
            for status in [
                GameStatus.DRAW,
                GameStatus.FIFTY_MOVER_RULE,
                GameStatus.INSUFFICIENT_MATERIAL,
                GameStatus.STALEMATE,
                GameStatus.THREEFOLD_REPETITION,
            ]
        ):
            return {"status": self.game_status, "winner": -1}
        else:
            return {"status": self.game_status, "winner": not self.turn}

    def timer_fun(self):
        self.times[self.turn] -= 1
        if self.times[self.turn] <= 0:
            self.end_game()
        else:
            self.socket.emit("timers", json.dumps(self.times), to=self.id)
