#include "Board.h"
#include <iterator>
#include <sstream>
#include <map>
Board::Board(int board[8][8], bool castling[4], int enPassant, bool sideToMove) {}
Board::Board(const Board& other) {}
Board::Board(std::string fen) {
    std::map<char, int> pieceMap
    { {'k',7},{'K',1},
        {'q',8},{'Q',2},
        {'n',9},{'N',3},
        {'b', 10}, {'B', 4},
        {'r', 11}, {'R', 5},
        {'p', 12}, {'P', 6}
    };
    std::map<char, int> castlingMap
    { {'k',2},{'K',0},
        {'q',3},{'Q',1},
    };

    std::istringstream iss(fen);
    std::istream_iterator<std::string> first(iss), last;
    std::vector<std::string> splitFen;// splitfen[0] - piece positions 1 -side to move, 2 - castling flags, 3 - enpassant destination, 4 - 75moverule, 5 - turn counter
    std::copy(first, last, std::back_inserter(splitFen));

    // parse pieces
    int row=0, col = 0;
    for (char character : splitFen[0]) {
        if (character != '/') {
            if (isdigit(character)) {
                col += (int)(character)-48;
            }
            else if (isalpha(character)) {
                this->board[row][col] = pieceMap[character];
                if (character == 'K') {
                    this->kingPos[0][0] = row;
                    this->kingPos[0][1] = col;
                }
                else if (character == 'k') {
                    this->kingPos[1][0] = row;
                    this->kingPos[1][1] = col;
                }
                col++;
            }
        }
        else {
            row++;
            col = 0;
        }
    }
    // /parse pieces

    // parse side to move
    this->sideToMove = splitFen[1] == "w" ? true : false;
    // /parse side to move
     
    // parse castling flags
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
    // /parse castling flags
    
    // parse en passant destination
    if (splitFen[3] != "-") {
        auto pos = ConvertStrToPosition(splitFen[3].c_str());
        this->enPassant[0] = pos[0];
        this->enPassant[1] = pos[1];
    }
    // /parse en passant destination
    
    // parse 75 move rule
    this->seventyFiveMoveRuleCounter = atoi(splitFen[4].c_str());
    // /parse 75 move rule
    
    // parse turn counter
    this->turnCounter = atoi(splitFen[5].c_str());
    // /parse turn counter
}

int* Board::ConvertStrToPosition(const char* pos) {
    std::map<char, int> columnMap {{'a',0},{'b',1},{'c',2},{'d',3},{'e',4},{'f',5},{'g',6},{'h',7}};
    std::map<char, int> rowMap{ {'8',0},{'7',1},{'6',2},{'5',3},{'4',4},{'3',5},{'2',6},{'1',7} };
    return new int[2]{rowMap[pos[1]], columnMap[pos[0]]};
}
char* Board::ConvertPositionToStr(int pos[2]) {
    std::map<int, char> columnMap{ {0,'a'},{1,'b'},{2,'c'},{3,'d'},{4,'e'},{5,'f'},{6,'g'},{7,'h'}};
    std::map<int, char> rowMap{ {0,'8'},{1,'7'},{2,'6'},{3,'5'},{4,'4'},{5,'3'},{6,'2'},{7,'1'}};
    return new char[2]{columnMap[pos[1]],rowMap[pos[0]]};
}