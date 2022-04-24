
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
    def __init__(self, fen: bytes) -> None:
        ...

    def make_move(self, move: Move) -> None:
        ...

    def get_all_legal_moves(self) -> map[Coordinates, list[Move]]:
        ...
