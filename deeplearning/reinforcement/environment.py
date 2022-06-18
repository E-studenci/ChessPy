import typing

import chesspy


class GameProvider:
    def __init__(self, fen_path: str) -> None:
        self.fen_path = fen_path
        self.file = None

    def __iter__(self):
        self.file = open(self.fen_path, "r", encoding="utf-8")
        return self

    def __next__(self) -> chesspy.Board:
        if not self.file:
            raise ValueError

        line = self.file.readline()
        if line:
            return chesspy.Board(line)
        else:
            self.file.close()
            self.file = None
            raise StopIteration


class GameManager:
    def __init__(
        self,
        board: chesspy.Board,
        engine: chesspy.SearchEngine,
        max_game_length: int,
        max_depth: int,
        max_move_time: int,
    ) -> None:
        self.board = board
        self.engine = engine
        self.max_game_length = max_game_length
        self.max_depth = max_depth
        self.max_move_time = max_move_time
        self.move_count = 0

    def __iter__(self):
        self.move_count = 0
        return self

    def __next__(self) -> chesspy.SearchResult:
        if self.move_count > self.max_game_length:
            raise StopIteration

        if len(self.board.get_all_legal_moves()) <= 0:
            raise StopIteration

        search_result = self.engine.root(self.board, self.max_depth, self.max_move_time)

        self.board.make_move(search_result.best_move)

        self.move_count += 1

        return search_result
