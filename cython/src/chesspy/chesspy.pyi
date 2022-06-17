import typing
import enum

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
    board: list[list[int]]
    fen_history: list[str]
    check: bool
    three_fold_repetition: bool
    fifty_move_rule_draw: bool
    fen: str
    
    def __init__(self, fen: bytes) -> None:
        ...

    def make_move(self, move: Move) -> None:
        ...
        
    def pop_move(self) -> None:
        ...
    
    def get_all_legal_moves(self) -> dict[Coordinates, list[Move]]:
        ...
        

class SearchResult:
    evaluation: int
    score_after_best_move: int
    reached_depth: int
    best_move: Move
    best_opponent_move: Move


class MoveOrderingType(enum.Enum):
    HANDCRAFTED = 0
    TRAINING = 1
    MODEL = 2


class SearchEngine:
    count: int
    
    def __init__(self, move_ordering_type: MoveOrderingType = None) -> None:
        ...

    def perft(self, board: Board, depth: int) -> int:
        ...
    
    def root(self, board: Board, depth: int, time_in_milliseconds: int, evaluatePosition = False, getOpponentBestMove = False) -> EvaluationResult:
        ...
