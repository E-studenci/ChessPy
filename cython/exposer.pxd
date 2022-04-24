from libcpp.string cimport string


cdef extern from "Coordinates.h":
    cdef cppclass CppCoordinates "Coordinates":
        CppCoordinates() except +
        CppCoordinates(int row, int column) except +
        int row
        int column


cdef extern from "Move.h":
    cdef cppclass CppMove "Move":
        CppMove(CppCoordinates origin, CppCoordinates destination, int promotion) except +
        CppCoordinates origin
        CppCoordinates destination
        int promotion


cdef extern from "Board.h":
    cdef cppclass CppBoard "Board":
        CppBoard() except +
        CppBoard(char *fen) except +
        void MakeMove(const CppMove move)
        string ToString()
