from libcpp.vector cimport vector
from libcpp.string cimport string

cdef extern from "Board.h":
    cdef cppclass Board:
        Board(char *fen) except +
        string ToString()


cdef class PyBoard:
    cdef Board*instance

    def __cinit__(self, char *fen):
        self.instance = new Board(fen)

    def __dealloc__(self):
        del self.instance

    def __str__(self):
        return self.instance.ToString().decode("utf-8")
