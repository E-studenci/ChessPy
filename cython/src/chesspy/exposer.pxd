from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.map cimport map
from libcpp cimport bool


cdef extern from "Coordinates.h":
    cdef cppclass CppCoordinates "Coordinates":
        CppCoordinates() except +
        CppCoordinates(int row, int column) except +
        int row
        int column


cdef extern from "Move.h":
    cdef cppclass CppMove "Move":
        CppMove(CppCoordinates origin, CppCoordinates destination, int promotion) except +
        CppMove* Clone()
        CppCoordinates origin
        CppCoordinates destination
        int promotion
        string ToString()


cdef extern from "Board.h":
    cdef cppclass CppBoard "Board":
        CppBoard() except +
        CppBoard(char *fen) except +
        bool KingInCheck(bool opponent)
        bool ThreeFoldRepetition()
        bool FifyMoveRuleDraw()
        void MakeMove(const CppMove &move, bool saveBoard)
        void Pop(bool saveBoard)
        map[CppCoordinates, vector[CppMove]] GetAllLegalMoves()
        string ToFen()
        string ToString()
        vector[vector[int]] GetBoard()
        vector[string] fenHistory


cdef extern from "SearchEngine.h":
    cdef cppclass CppSearchResult "SearchResult":
        CppSearchResult() except +
        CppSearchResult(int reachedDepth, CppMove bestMove, int scoreAfterBestMove, int evaluation, CppMove bestOpponentMove, vector[int] nodeCount) except +
        int evaluation
        int scoreAfterBestMove
        int reachedDepth
        CppMove bestMove
        CppMove bestOpponentMove
        vector[int] nodeCount

    cdef cppclass CppSearchEngine "SearchEngine":
        CppSearchEngine() except +
        CppSearchEngine(int moveOrderer) except +
        int PerftStarterSingleThread(CppBoard* board, int depth)
        CppSearchResult Root(CppBoard* board, int depth, long timeInMillis, bool evaluatePosition, bool getOpponentBestMove)
        int AlphaBeta(CppBoard* board, int alpha, int beta, int depthLeft)
        int count
