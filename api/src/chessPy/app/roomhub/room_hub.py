import flask_socketio as fs
from flask import Flask
from enum import Enum
import threading

import uuid
from chessPy.app.roomhub.room import Room
from cython import chess
class GameStatus(Enum):
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
    open_rooms: list['Room'] = {}
    rooms: dict[uuid.UUID, 'Room'] = {}
    socket = fs.SocketIO()
    def __init__(self, flask_app: Flask) -> None:
        self.socket = fs.SocketIO(flask_app)

    @socket.on('find_game')
    def find_game(self): #TODO: add options (game length)
        if len(self.open_rooms > 0):
            game_room = self.open_rooms.pop()
            self.socket.join_room(game_room.id)
            self.socket.emit("find_game", {"found": True,
                                  "message": "Found an opponent"}, to=game_room.id)
            game_room.add_player(fs.request.sid)
            self.rooms[game_room.id] = game_room
            self.start_room(game_room.id)
        else: 
            game_room = Room(uuid.uuid1, fs.request.sid, self.socket)
            self.open_rooms.append(game_room)
            self.socket.join_room(game_room.id)
            self.socket.emit("find_game", {"found": False,
                                  "message": "Looking for an opponent"}, to=fs.request.sid)

    def start_room(self, game_room_id):
        res = self.rooms[game_room_id].start_game()
        self.socket.emit("game_started", {"color": 0, "legal_moves": res[1]},to=res[0][0])
        self.socket.emit("game_started", {"color": 1},to=res[0][1])


    @socket.on("make_move")
    def make_move(self, data):
        room_id = data["room_id"]
        if not self.rooms[room_id]: # the room doesn't exist
            self.socket.emit("make_move_error", {"message": "The game does not exist"}, to=fs.request.sid)
            return
        else:
            result = self.rooms[room_id].make_move_check(fs.request.sid, data["move"])
            if not result[0]: # the move wasn't legal
                self.socket.emit("make_move_error", {"message": result[1]}, to=fs.request.sid)
            else:
                self.rooms[room_id].make_move(data["move"])
                res = self.rooms[room_id].end_turn()
                if not res[0]: # the game ended after the move
                    game_status = self.rooms[room_id].end_game()
                    self.socket.emit("game_ended", game_status, to=room_id)
                    self.socket.close_room(room_id)
                else:
                    self.socket.emit("turn_ended", res[1], to=room_id) # TODO: add king checks to ret
    
    @socket.on('disconnect')
    def disconnected(self):
        for i, room in enumerate(self.open_rooms):
            if fs.request.sid in room.players:
                self.socket.close_room(room.id)
                self.open_rooms.remove(i)
        for room in self.rooms.values():
            if fs.request.sid in room.players:
                room.end_game()
                self.socket.emit("game_ended", {"status": GameStatus.DISCONNECTED,
                                                "winner": 1 if room.players[0] == fs.request.sid else 0}, to=room.id)
                self.socket.close_room(room.id)
                self.rooms.pop(room.id)


class RepeatTimer(threading.Timer):
    def run(self):
        while not self.finished.wait(self.interval):
            self.function(*self.args, **self.kwargs)
            
