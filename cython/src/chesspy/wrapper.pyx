# cython: c_string_type=unicode, c_string_encoding=utf8
from cython.operator cimport dereference as deref

from exposer cimport CppCoordinates, CppMove, CppBoard, CppSearchResult, CppSearchEngine

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
    

cdef class Board:
    cdef CppBoard*instance

    def __cinit__(self, char *fen):
        self.instance = new CppBoard(fen)

    def __dealloc__(self):
        del self.instance

    def __str__(self):
        return self.instance.ToString()
    
    @property
    def board(self):
        return self.instance.GetBoard()

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
    
    def make_move(self, Move move):
        self.instance.MakeMove(deref(move.instance), True)

    def pop_move(self):
        self.instance.Pop(True)
    
    def get_all_legal_moves(self):
        result = MoveDict()
        mappings = self.instance.GetAllLegalMoves()
        for pair in mappings:
            cords = pair.first
            moves = pair.second
            converted_moves = []
            for i in range(moves.size()):
                x = moves[i].origin.row
                py_move = Move(
                    Coordinates(moves[i].origin.row, moves[i].origin.column),
                    Coordinates(moves[i].destination.row, moves[i].destination.column),
                    moves[i].promotion
                ) 
                converted_moves.append(py_move)
            result[Coordinates(cords.row, cords.column)] = converted_moves
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


class MoveOrderingType(enum.Enum):
    HANDCRAFTED = 0
    TRAINING = 1
    MODEL = 2


cdef class SearchEngine:
    cdef CppSearchEngine*instance

    def __cinit__(self, move_ordering_type: MoveOrderingType = None):
        if move_ordering_type:
            self.instance = new CppSearchEngine(move_ordering_type)
        else:
            self.instance = new CppSearchEngine()

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
        return SearchResult(result.evaluation, result.scoreAfterBestMove, result.reachedDepth, bestMove, bestOpponentMove, result.nodeCount)

try:
    import exposed
    EVAL_FUNC = exposed.evaluate_move
except:
    EVAL_FUNC = None

cdef public double evaluateMove(const CppBoard* board, CppMove* move):
    if EVAL_FUNC:
        return EVAL_FUNC()
    return 0