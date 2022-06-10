from chesspy_api.app.roomhub.room_hub import RepeatTimer
from chesspy_api.app.roomhub.room_hub import GameStatus
import flask_socketio as fs
from cython import chess
import random

class Room:
    game_status: GameStatus
    socket = None
    timer = RepeatTimer(1, print)

    def __init__(self, id, player_one, socket:fs.SocketIO, fen: str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"):
        self.id = id
        self.players = [player_one]
        self.board: chess.Board = chess.Board(fen)
        self.socket = socket
        self.time_after_turn = 0
    
    def add_player(self, player_two):
        self.players.append(player_two)

    def start_game(self):
        self.turn = 0
        random.shuffle(self.players)
        self.times = [600,600]
        self.timer = RepeatTimer(1, self.timer_fun)
        self.timer.start()
        return (self.players, self.board.get_all_legal_moves())

    def make_move_check(self,player_id, chosen_move:chess.Move):
        if not any(player == player_id for player in self.players):
            return (False, "It's not your game")
        if self.players[self.turn] != player_id:
            return (False, "It's not your turn")
        if self.game_status != GameStatus.ONGOING:
            return (False, "The game is not active")
        if not any(chosen_move in moves for moves in self.board.get_all_legal_moves().values()):
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
            return (False, self.board.get_board)
        else:
            self.timer = RepeatTimer(1, self.timer_fun)
            self.timer.start()
            return (True, {"board": self.board.get_board,
                           "turn": self.turn,
                           "legal_moves": self.board.get_legal_moves,
                           "king_in_check": self.board.KingInCheck})

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
        try: self.timer.cancel()
        except: pass
        if any(time <= 0 for time in self.times):
            self.game_status = GameStatus.TIME_RUN_OUT
            return {"status": self.game_status, "winner": 1 if self.timers[0]<=0 else 0}
        elif any(self.game_status == status for status in [GameStatus.DRAW, GameStatus.FIFTY_MOVES, GameStatus.INSUFFICIENT_MATERIAL, GameStatus.STALEMATE, GameStatus.THREEFOLD_REPETITION]):
            return {"status": self.game_status, "winner": -1}
        else:
            return {"status": self.game_status, "winner": not self.turn}
        

    def timer_fun(self):
        self.timers[self.turn] -= 1
        if self.timers[self.turn] <= 0:
            self.end_game()
        else:
            self.socket.emit("timers", self.times, to=self.id)

