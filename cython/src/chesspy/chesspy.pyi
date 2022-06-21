import typing
import enum

class Coordinates:
    row: int
    column: int

    def __init__(self, row: int = None, column: int = None) -> None: ...

class Move:
    origin: Coordinates
    destination: Coordinates
    promotion: int

    def __init__(
        self, origin: Coordinates, destination: Coordinates, promotion: int = 0
    ) -> None: ...

class GameStatus(enum.IntEnum):
    ONGOING = 0
    STALEMATE = 1
    MATE = 2
    THREEFOLD_REPETITION = 3
    FIFTY_MOVER_RULE = 4
    INSUFFICIENT_MATERIAL = 5

class Board:
    board: list[list[int]]
    game_status: GameStatus
    fen_history: list[str]
    check: bool
    three_fold_repetition: bool
    fifty_move_rule_draw: bool
    fen: str
    hash: str

    def __init__(self, fen: bytes) -> None: ...
    def make_move(self, move: Move) -> None: ...
    def pop_move(self) -> None: ...
    def get_all_legal_moves(self) -> list[Move]: ...

class SearchResult:
    evaluation: int
    score_after_best_move: int
    reached_depth: int
    best_move: Move
    best_opponent_move: Move
    node_count: list[int]

class MoveOrderingType(enum.IntEnum):
    HANDCRAFTED = 0
    TRAINING = 1
    MODEL = 2


class SearchParams:
    def __init__(
        self,
        use_null_move_pruning: bool = False,
        use_killer_moves: bool = True,
        use_hashed_positions: bool = True,
        use_hashed_moves: bool = True,
        use_quiescence: bool = True,
        use_check_extension: bool = True,
        use_MVVLVA: bool = True
    ) -> None: ...


class SearchEngine:
    count: int

    def __init__(
        self,
        move_ordering_type: MoveOrderingType = MoveOrderingType.HANDCRAFTED,
        search_params: SearchParams = SearchParams(),
    ) -> None: ...
    def perft(self, board: Board, depth: int) -> int: ...
    def root(
        self,
        board: Board,
        depth: int,
        time_in_milliseconds: int,
        evaluatePosition=False,
        getOpponentBestMove=False,
    ) -> SearchResult: ...
