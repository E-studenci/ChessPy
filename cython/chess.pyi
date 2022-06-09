import typing

class Coordinates:
    row: int
    column: int

    def __init__(self, row: int = None, column: int = None) -> None:
        ...


class Move:
    origin: Coordinates
    destination: Coordinates
    promotion: int

    def __init__(self, origin: Coordinates, destination: Coordinates, promotion: int = 0) -> None:
        ...


class Board:
    check: bool
    three_fold_repetition: bool
    fifty_move_rule_draw: bool
    fen: str
    
    def __init__(self, fen: bytes) -> None:
        ...

    def make_move(self, move: Move) -> None:
        ...

    def get_all_legal_moves(self) -> map[Coordinates, list[Move]]:
        ...
        


class Algorithms:
    count: int
    
    def __init__(self) -> None:
        ...

    def perft(self, board: Board, depth: int) -> int:
        ...
    
    def root(self, board: Board, depth: int, time_in_milliseconds: int) -> typing.Tuple[Move, int, int]:
        ...
