#include "Board.h"
Board::Board(int board[8][8], bool castling[4], int enPassant, bool sideToMove) {}
Board::Board(const Board& other) {}
Board::Board(std::string fen) {
    std::map<char, int> pieceMap
    { {'k',6},{'K',0},
        {'q',7},{'Q',1},
        {'n',8},{'N',2},
        {'b', 9}, {'B', 3},
        {'r', 10}, {'R', 4},
        {'p', 11 }, {'P', 5}
    };
    std::map<char, int> castlingMap
    { {'k',2},{'K',0},
        {'q',3},{'Q',1},
    };
    char* splitFen = strtok(fen.data(), " "); // splitfen[0] - piece positions 1 -side to move, 2 - castling flags, 3 - enpassant destination, 4 - 75moverule, 5 - turn counter
    this->sideToMove = splitFen[1] == 'w' ? true : false;
}
Board::~Board() {}