import typing
import random

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
        self.move = None

    def __iter__(self):
        self.move_count = 0
        self.move = None
        return self

    def __next__(self) -> chesspy.SearchResult:
        if self.move_count > self.max_game_length:
            raise StopIteration

        if self.board.game_status != chesspy.GameStatus.ONGOING:
            raise StopIteration

        if self.move:
            self.board.make_move(self.move)

        search_result = self.engine.root(self.board, self.max_depth, self.max_move_time)

        self.move = search_result.best_move

        self.move_count += 1

        return search_result


class Awarder:
    def __init__(self, no_change_range: int) -> None:
        self.no_change_range = no_change_range

    def award(self, current_number_of_nodes: int, previous_results: list):
        if not previous_results:
            return random.randint(-5, 5)

        previous_nodes = list(map(lambda item: item[0], previous_results))

        average = sum(previous_nodes) / len(previous_nodes)
        if (
            current_number_of_nodes < average + self.no_change_range
            and current_number_of_nodes > average - self.no_change_range
        ):
            return 0
        elif current_number_of_nodes > average + self.no_change_range:
            return -10
        elif current_number_of_nodes < average - self.no_change_range:
            return 10
        return 0
