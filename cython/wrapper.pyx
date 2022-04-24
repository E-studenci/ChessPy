from cython.operator cimport dereference as deref
from libcpp.vector cimport vector
from libcpp.string cimport string

from exposer cimport CppCoordinates, CppMove, CppBoard


cdef class Coordinates:
    cdef CppCoordinates*instance

    def __cinit__(self, row=None, column=None):
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



cdef class Move:
    cdef CppMove*instance

    def __cinit__(self, Coordinates origin, Coordinates destination, int promotion = 0):
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
        return f"Move(origin={self.origin},destination={self.destination},promotion={self.promotion})"


cdef class Board:
    cdef CppBoard*instance

    def __cinit__(self, char *fen):
        self.instance = new CppBoard(fen)

    def __dealloc__(self):
        del self.instance

    def __str__(self):
        return self.instance.ToString().decode("utf-8")

    def make_move(self, Move move):
        self.instance.MakeMove(deref(move.instance))
