# cython: c_string_type=unicode, c_string_encoding=utf8
from cython.operator cimport dereference as deref

from exposer cimport CppCoordinates, CppMove, CppBoard, CppSearchResult, CppSearchParams, CppSearchEngine

import enum

cdef class Coordinates:
    cdef CppCoordinates*instance

    def __init__(self, row=None, column=None):
        if row is not None and column is not None:
            self.instance = new CppCoordinates(row, column)
        else:
            self.instance = new CppCoordinates()

    def __dealloc__(self):
        del self.instance

    @property
    def row(self):
        return self.instance.row
    
    @row.setter
    def row(self, row: int):
        self.instance.row = row

    @property
    def column(self):
        return self.instance.column
    
    @column.setter
    def column(self, column: int):
        self.instance.column = column
    
    def __str__(self) -> str:
        return f"Coordinates(row={self.row},column={self.column})"

    def __hash__(self):
        return hash((self.row, self.column))

    def __eq__(self, other):
        return (self.row, self.column) == (other.row, other.column)

    def __ne__(self, other):
        return not self == other


cdef class Move:
    cdef CppMove*instance

    def __init__(self, Coordinates origin = None, Coordinates destination = None, int promotion = 0):
        if origin is not None and destination is not None:
            self.instance = new CppMove(deref(origin.instance), deref(destination.instance), promotion)
    
    def __dealloc__(self):
        del self.instance
    
    @property
    def origin(self):
        return Coordinates(self.instance.origin.row, self.instance.origin.column)
    
    @origin.setter
    def origin(self, Coordinates origin):
        self.instance.origin = deref(origin.instance)
    
    @property
    def destination(self):
        return Coordinates(self.instance.destination.row, self.instance.destination.column)
    
    @destination.setter
    def destination(self, Coordinates destination):
        self.instance.destination = deref(destination.instance)

    @property
    def promotion(self):
        return self.instance.promotion
    
    @promotion.setter
    def promotion(self, int promotion):
        self.instance.promotion = promotion
    
    def __str__(self) -> str:
        return self.instance.ToString()


class MoveDict(dict):
    def __getitem__(self, val):
        if isinstance(val, tuple):
            val = Coordinates(*val)
        return super().__getitem__(val)


class GameStatus(enum.IntEnum):
    ONGOING = 0
    STALEMATE = 1
    MATE = 2
    THREEFOLD_REPETITION = 3
    FIFTY_MOVER_RULE = 4
    INSUFFICIENT_MATERIAL = 5


cdef class Board:
    cdef CppBoard*instance

    def __cinit__(self, fen='rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', init=True):
        if init:
            self.instance = new CppBoard(fen)

    def __dealloc__(self):
        del self.instance

    def __str__(self):
        return self.instance.ToString()
    
    @property
    def board(self):
        return self.instance.GetBoard()
    
    @property
    def game_status(self):
        return GameStatus(self.instance.GetGameStatus())

    @property
    def fen_history(self):
        return self.instance.fenHistory

    @property
    def check(self):
        return self.instance.KingInCheck(False)
    
    @property
    def three_fold_repetition(self):
        return self.instance.ThreeFoldRepetition()

    @property
    def fifty_move_rule_draw(self):
        return self.instance.FifyMoveRuleDraw()

    @property
    def fen(self):
        return self.instance.ToFen()

    @property
    def hash(self):
        return self.instance.GetCurrentKey()
    
    def make_move(self, Move move):
        self.instance.MakeMove(deref(move.instance), True)

    def pop_move(self):
        self.instance.Pop(True)
    
    def get_all_legal_moves(self):
        result = []
        moves = self.instance.GetAllLegalMoves()
        for i in range(moves.size()):
            py_move = Move()
            py_move.instance = moves[i].Clone()
            result.append(py_move)
        return result


class SearchResult:
    evaluation: int
    score_after_best_move: int
    reached_depth: int
    best_move: Move
    best_opponent_move: Move
    node_count: list[int]


    def __init__(self, evaluation, score_after_best_move, reached_depth, best_move, best_opponent_move, node_count):
        self.evaluation = evaluation
        self.score_after_best_move = score_after_best_move
        self.reached_depth = reached_depth
        self.best_move = best_move
        self.best_opponent_move = best_opponent_move
        self.node_count = node_count


class MoveOrderingType(enum.IntEnum):
    HANDCRAFTED = 0
    TRAINING = 1
    MODEL = 2

cdef class SearchParams:
    cdef CppSearchParams*instance

    def __cinit__(
        self, 
        use_null_move_pruning: bool = False, 
        use_killer_moves: bool = True, 
        use_hashed_positions: bool = True, 
        use_hashed_moves: bool = True, 
        use_quiescence: bool = True, 
        use_check_extension: bool = True, 
        use_MVVLVA: bool = True
    ):
        self.instance = new CppSearchParams(
            use_null_move_pruning, 
            use_killer_moves,
            use_hashed_positions,
            use_hashed_moves,
            use_quiescence,
            use_check_extension,
            use_MVVLVA
        )
    
    def __dealloc__(self):
        del self.instance


cdef class SearchEngine:
    cdef CppSearchEngine*instance

    def __cinit__(self, move_ordering_type: MoveOrderingType = MoveOrderingType.HANDCRAFTED, serch_params: SearchParams = SearchParams()):
        self.instance = new CppSearchEngine(int(move_ordering_type), deref(serch_params.instance))

    def __dealloc__(self):
        del self.instance
        
    @property
    def count(self):
        return self.instance.count
    
    def perft(self, Board board, int depth) -> int:
        return self.instance.PerftStarterSingleThread(board.instance, depth)
    
    def root(self, Board board, int depth, long timeInMillis, evaluatePosition = False, getOpponentBestMove = False):
        result = self.instance.Root(board.instance, depth, timeInMillis, evaluatePosition, getOpponentBestMove)
        bestMove = Move()
        bestOpponentMove = Move()
        bestMove.instance = result.bestMove.Clone()
        bestOpponentMove.instance = result.bestOpponentMove.Clone()
        return SearchResult(
            result.evaluation,
            result.scoreAfterBestMove,
            result.reachedDepth,
            bestMove,
            bestOpponentMove,
            result.nodeCount
        )


cdef public double evaluateMove(CppBoard* board, CppMove* move):
    py_board = Board(init=False)
    py_board.instance = board.Clone()
    py_move = Move()
    py_move.instance = move.Clone()

    try:
        import exposed
        return exposed._FUNC(py_board, py_move)
    except:
        return 0.0
