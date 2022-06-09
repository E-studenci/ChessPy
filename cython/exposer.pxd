from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.map cimport map
from libcpp cimport bool


cdef extern from "<array>" namespace "std" nogil:
    cdef cppclass array8 "std::array<int, 8>":
        array8() except+
        int& operator[](size_t)


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


cdef extern from "Board.h":
    cdef cppclass CppBoard "Board":
        CppBoard() except +
        CppBoard(char *fen) except +
        bool KingInCheck(bool opponent)
        bool ThreeFoldRepetition()
        bool FifyMoveRuleDraw()
        void MakeMove(const CppMove &move)
        map[CppCoordinates, vector[CppMove]] GetAllLegalMoves()
        string ToFen()
        string ToString()


cdef extern from "Algorithms.h":
    cdef cppclass CppAlgorithms "Algorithms":
        CppAlgorithms() except +
        int PerftStarterSingleThread(CppBoard* board, int depth)
        pair[CppMove, pair[int, int]] Root(CppBoard* board, int depth, long timeInMillis)
        int AlphaBeta(CppBoard* board, int alpha, int beta, int depthLeft)
        int count
