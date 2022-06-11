import uuid
import enum
import random
import threading

import flask
import chess
import flask_socketio as fs


class GameStatus(enum.Enum):
    ONGOING = 0
    CHECKMATE = 1
    STALEMATE = 2
    FIFTY_MOVES = 3
    THREEFOLD_REPETITION = 4
    INSUFFICIENT_MATERIAL = 5
    DRAW = 6
    TIME_RUN_OUT = 7
    DISCONNECTED = 8


class RoomHub:
    open_rooms: list["Room"] = {}
    rooms: dict[uuid.UUID, "Room"] = {}
    socket = fs.SocketIO()

    def __init__(self, flask_app: flask.Flask) -> None:
        self.socket = fs.SocketIO(
            flask_app, logger=True, engineio_logger=True, cors_allowed_origins="*"
        )  # TODO: TEMP

    @socket.on("find_game")
    def find_game(self):  # TODO: add options (game length)
        if len(self.open_rooms > 0):
            game_room = self.open_rooms.pop()
            self.socket.join_room(game_room.id)
            self.socket.emit(
                "find_game",
                {"found": True, "message": "Found an opponent"},
                to=game_room.id,
            )
            game_room.add_player(fs.request.sid)
            self.rooms[game_room.id] = game_room
            self.start_room(game_room.id)
        else:
            game_room = Room(uuid.uuid1, fs.request.sid, self.socket)
            self.open_rooms.append(game_room)
            self.socket.join_room(game_room.id)
            self.socket.emit(
                "find_game",
                {"found": False, "message": "Looking for an opponent"},
                to=fs.request.sid,
            )

    def start_room(self, game_room_id):
        res = self.rooms[game_room_id].start_game()
        self.socket.emit(
            "game_started", {"color": 0, "legal_moves": res[1]}, to=res[0][0]
        )
        self.socket.emit("game_started", {"color": 1}, to=res[0][1])

    @socket.on("make_move")
    def make_move(self, data):
        room_id = data["room_id"]
        if not self.rooms[room_id]:  # the room doesn't exist
            self.socket.emit(
                "make_move_error",
                {"message": "The game does not exist"},
                to=fs.request.sid,
            )
            return
        else:
            result = self.rooms[room_id].make_move_check(fs.request.sid, data["move"])
            if not result[0]:  # the move wasn't legal
                self.socket.emit(
                    "make_move_error", {"message": result[1]}, to=fs.request.sid
                )
            else:
                self.rooms[room_id].make_move(data["move"])
                res = self.rooms[room_id].end_turn()
                if not res[0]:  # the game ended after the move
                    game_status = self.rooms[room_id].end_game()
                    self.socket.emit("game_ended", game_status, to=room_id)
                    self.socket.close_room(room_id)
                else:
                    # TODO: add king checks to ret
                    self.socket.emit("turn_ended", res[1], to=room_id)

    @socket.on("disconnect")
    def disconnected(self):
        for i, room in enumerate(self.open_rooms):
            if fs.request.sid in room.players:
                self.socket.close_room(room.id)
                self.open_rooms.remove(i)
        for room in self.rooms.values():
            if fs.request.sid in room.players:
                room.end_game()
                self.socket.emit(
                    "game_ended",
                    {
                        "status": GameStatus.DISCONNECTED,
                        "winner": 1 if room.players[0] == fs.request.sid else 0,
                    },
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
        player_one,
        socket: fs.SocketIO,
        fen: str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    ):
        self.id = id
        self.players = [player_one]
        self.board = chess.Board(fen)
        self.socket = socket
        self.time_after_turn = 0

    def add_player(self, player_two):
        self.players.append(player_two)

    def start_game(self):
        self.turn = 0
        random.shuffle(self.players)
        self.times = [600, 600]
        self.timer = RepeatTimer(1, self.timer_fun)
        self.timer.start()
        return (self.players, self.board.get_all_legal_moves())

    def make_move_check(self, player_id, chosen_move: chess.Move):
        if not any(player == player_id for player in self.players):
            return (False, "It's not your game")
        if self.players[self.turn] != player_id:
            return (False, "It's not your turn")
        if self.game_status != GameStatus.ONGOING:
            return (False, "The game is not active")
        if not any(
            chosen_move in moves for moves in self.board.get_all_legal_moves().values()
        ):
            return (False, "No cheating!")

    def make_move(self, chosen_move):
        self.board.make_move(chosen_move)
        self.timer.cancel()
        self.times[self.turn] += self.time_after_turn

    def end_turn(self):
        self.check_game_ending_conditions()
        self.game_status = self.board.game_status
        self.turn = int(not self.turn)
        if self.game_status != GameStatus.ONGOING:
            return (False, self.board)
        else:
            self.timer = RepeatTimer(1, self.timer_fun)
            self.timer.start()
            return (
                True,
                {
                    "board": self.board.board,
                    "turn": self.turn,
                    "legal_moves": self.board.get_all_legal_moves(),
                    "king_in_check": self.board.check,
                },
            )

    def check_game_ending_conditions(self):
        """
        :param player: current player
        :return: returns:
                GameStatus.FIFTY_MOVES            : if the fifty move rule has been broken;
                GameStatus.CHECKMATE              : if the current player is in mate;
                GameStatus.THREEFOLD_REPETITION   : if the threefold repetition rule has been broken;
                GameStatus.INSUFFICIENT_MATERIAL  : if neither player can possibly mate the opposing player;
                GameStatus.ONGOING                : otherwise
        """
        player_move_count = len(self.board.get_all_legal_moves().values())
        if self.board.fifty_move_rule_draw():
            self.game_status = GameStatus.FIFTY_MOVES
        elif player_move_count == 0:
            if self.board.check:
                self.game_status = GameStatus.CHECKMATE
            else:
                self.game_status = GameStatus.STALEMATE
        elif self.board.three_fold_repetition():
            self.game_status = GameStatus.THREEFOLD_REPETITION
        # elif self.InsufficientMaterial(): TODO
        #     self.game_status = GameStatus.INSUFFICIENT_MATERIAL
        else:
            self.game_status = GameStatus.ONGOING

    def end_game(self):
        try:
            self.timer.cancel()
        except:
            pass
        if any(time <= 0 for time in self.times):
            self.game_status = GameStatus.TIME_RUN_OUT
            return {
                "status": self.game_status,
                "winner": 1 if self.timers[0] <= 0 else 0,
            }
        elif any(
            self.game_status == status
            for status in [
                GameStatus.DRAW,
                GameStatus.FIFTY_MOVES,
                GameStatus.INSUFFICIENT_MATERIAL,
                GameStatus.STALEMATE,
                GameStatus.THREEFOLD_REPETITION,
            ]
        ):
            return {"status": self.game_status, "winner": -1}
        else:
            return {"status": self.game_status, "winner": not self.turn}

    def timer_fun(self):
        self.timers[self.turn] -= 1
        if self.timers[self.turn] <= 0:
            self.end_game()
        else:
            self.socket.emit("timers", self.times, to=self.id)
