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

void Board::Capture(int destination[2]) {}
void Board::MakeMove(Move move) {
    int movingPiece = this->board[move.from[0]][move.from[1]];
    bool resetEnPassant = true;
    
    this->seventyFiveMoveRuleCounter++;

    // move is a take
    if (this->board[move.destination[0]][move.destination[1]] != 0) {
        this->Capture(move.destination);
    }
    // /move is a take
    // move is made by a pawn
    if (movingPiece == 6 || movingPiece == 12) {
        this->seventyFiveMoveRuleCounter = 0;
        // en passant
        if (move.destination == this->enPassant) {
            this->Capture(new int[] {move.destination[0] + (movingPiece==6? 1 : -1), move.destination[1]}); // capture the en passant victim
        }
        // /en passant
        // first move
        if ((move.from[0] == 1 && movingPiece == 12) ||
            (move.from[0] == 6 && movingPiece == 6)) {
            this->enPassant[0] = move.from[0] == 1 ? 2 : 5; // TODO: this could potentially be dangerous, if so, check if there is an adjacent enemy pawn
            resetEnPassant = false;
        }
        // /first move
        // promotion
        if (move.promotion != 0) {
            this->board[move.from[0]][move.from[1]] = move.promotion;
        }
        // /promotion
    }
    // /move is made by a pawn
    // move is made by a king
    else if (movingPiece == 1 || movingPiece == 7) {
        // castle
        if (move.from[1] - move.destination[1] == -2) { // king side castle, move the rook to it's final value
            this->board[move.from[0]][move.from[1] + 1] = this->board[move.from[0]][move.from[1] + 3];
            this->board[move.from[0]][move.from[1] + 3] = 0;
        } 
        else if (move.from[1] - move.destination[1] == 2) {// queen side castle, move the rook to it's final value
            this->board[move.from[0]][move.from[1] - 1] = this->board[move.from[0]][move.from[1] - 4];
            this->board[move.from[0]][move.from[1] - 4] = 0;
        } 
        // /castle
        // reset castle flags
        if (movingPiece == 1) {
            this->castlingFlags[0] = false;
            this->castlingFlags[1] = false;
        }
        else if (movingPiece == 7) {
            this->castlingFlags[2] = false;
            this->castlingFlags[3] = false;
        }
        // /reset castle flags
    }
    // /move is made by a king
    // move is made by a rook
    else if (movingPiece == 5 || movingPiece == 11) {
        if (this->castlingFlags[0] && move.from[0] == 7 && move.from[1] == 7) { this->castlingFlags[0] = false; } // reset wk castle flag
        else if (this->castlingFlags[1] && move.from[0] == 7 && move.from[1] == 0) {this->castlingFlags[1] = false;} // reset wq castle flag
        else if (this->castlingFlags[2] && move.from[0] == 0 && move.from[1] == 7) {this->castlingFlags[2] = false;} // reset bk castle flag
        else if (this->castlingFlags[3] && move.from[0] == 0 && move.from[1] == 0) {this->castlingFlags[3] = false;} // reset bq castle flag
    }
    // /move is made by a rook
    // reset en passant flag
    if (resetEnPassant) {
        this->enPassant[0] = -1;
        this->enPassant[1] = -1;
    }
    // /reset en passant flag
    // move the piece
    this->board[move.destination[0]][move.destination[1]] = this->board[move.from[0]][move.from[1]];
    this->board[move.from[0]][move.from[1]] = 0;
    // /move the piece
    // save the move in move history
    // /save the move in move history
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