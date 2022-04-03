#include "Board.h"
#include <iterator>
#include <sstream>
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
    std::istringstream iss(fen);
    std::istream_iterator<std::string> first(iss), last;

    std::vector<std::string> splitFen;// splitfen[0] - piece positions 1 -side to move, 2 - castling flags, 3 - enpassant destination, 4 - 75moverule, 5 - turn counter
    std::copy(first, last, std::back_inserter(splitFen));
    this->sideToMove = splitFen[1] == "w" ? true : false;
    if (splitFen[2] == "-") {
        this->castlingFlags[0] = false;
        this->castlingFlags[1] = false;
        this->castlingFlags[2] = false;
        this->castlingFlags[3] = false;
    }
    else {
        for (const auto& flag : splitFen[2]) {
            this->castlingFlags[castlingMap[flag]] = true;
        }
    }
}