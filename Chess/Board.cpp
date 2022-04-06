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

    //parse pieces
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
    else {
        this->enPassant[0] = -1;
        this->enPassant[1] = -1;
    }
    // /parse en passant destination
    
    // parse 75 move rule
    this->seventyFiveMoveRuleCounter = atoi(splitFen[4].c_str());
    // /parse 75 move rule
    
    // parse turn counter
    this->turnCounter = atoi(splitFen[5].c_str());
    // /parse turn counter
}

void Board::Capture(int destination[2]) {
    // remove the piece from the board
    this->board[destination[0]][destination[1]] = 0;
    // /remove the piece from the board
    // reset the 75move rule counter
    this->seventyFiveMoveRuleCounter = 0;
    // /reset the 75move rule counter
    // take care of castling flags
    if (this->board[destination[0]][destination[1]] == 5 || this->board[destination[0]][destination[1]] == 11) {
        
    }
    // /take care of castling flags
}
void Board::MakeMove(Move move) {
    int movingPiece = this->board[move.from[0]][move.from[1]];
    bool resetEnPassant = true;

    // append a copy of this->board to board history
    int** boardCopy = new int*[8];
    for (int i = 0;i < 8;i++) {
        boardCopy[i] = new int[8];
    }
    std::copy(&this->board[0][0], &this->board[0][0] + 8 * 8, &boardCopy[0][0]);
    this->boardHistory.push_back(boardCopy);
    // /append a copy of this->board to board history

    // prepare variables for move history, probably temporary 
    // TODO: I'm a dum dum, remove some trash from here
    bool castlingFlags[4];
    std::copy(std::begin(this->castlingFlags), std::end(this->castlingFlags), std::begin(castlingFlags));
    int enPassant[2];
    std::copy(std::begin(this->enPassant), std::end(this->enPassant), std::begin(enPassant));
    int seventyFiveMoveRule = this->seventyFiveMoveRuleCounter;
    // /prepare variables for move history

    // move is a take
    if (this->board[move.destination[0]][move.destination[1]] != 0) {
        this->Capture(move.destination);
    }
    // /move is a take
    // move is made by a pawn
    if (movingPiece == 6 || movingPiece == 12) {
        this->seventyFiveMoveRuleCounter = 0;
        // en passant
        if (move.destination[0] == this->enPassant[0] && move.destination[1] == this->enPassant[1]) {
            int enPassantVictimPosition[2];
            enPassantVictimPosition[0] = move.destination[0] + (movingPiece == 6 ? 1 : -1);
            enPassantVictimPosition[1] = move.destination[1];
            this->Capture(enPassantVictimPosition); // capture the en passant victim
        }
        // /en passant
        // first move
        if ((move.from[0] == 1 && movingPiece == 12) ||
            (move.from[0] == 6 && movingPiece == 6)) {
            this->enPassant[0] = move.from[0] == 1 ? 2 : 5; // TODO: this could potentially be dangerous, if so, check if there is an adjacent enemy pawn
            this->enPassant[1] = move.from[1]; // TODO: this could potentially be dangerous, if so, check if there is an adjacent enemy pawn

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
    // move is made by a king TODO: update this->kingPos
    else if (movingPiece == 1 || movingPiece == 7) {
        bool kingSideCastle = move.from[1] - move.destination[1] == -2;
        bool queenSideCastle = move.from[1] - move.destination[1] == 2;
        // castling
        if (kingSideCastle || queenSideCastle) {
            int castleFromCol = move.from[1] + kingSideCastle ? 3 : -4;
            int castleDestCol = move.from[1] + kingSideCastle ? 1 : -1;
            this->board[move.from[0]][castleDestCol] = this->board[move.from[0]][castleFromCol];
            this->board[move.from[0]][castleFromCol] = 0;
        }
        // /castling
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
        // update king position
        this->kingPos[movingPiece == 1?0:1][0] = move.destination[0];
        this->kingPos[movingPiece == 1?0:1][1] = move.destination[1];
        // /update king position

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
    Move m(move.from, move.destination, move.promotion, castlingFlags, enPassant, seventyFiveMoveRule);
    this->moveHistory.push_back(m);
    // /save the move in move history
    if (!sideToMove) {
        this->turnCounter++;
    }
    this->seventyFiveMoveRuleCounter++;
    this->sideToMove = !this->sideToMove;
}
void Board::Pop(){
    // revert the changes made by the move
    int boardIndex = this->boardHistory.size() - 1;
    int moveIndex = this->moveHistory.size() - 1;
    for (int i =0; i<8; i++){
        for(int j=0; j<8;j++){
            this->board[i][j] = this->boardHistory[boardIndex][i][j];
        }
    }
    this->seventyFiveMoveRuleCounter = this->moveHistory[moveIndex].seventyFiveMoveRule;
    this->enPassant[0] = this->moveHistory[moveIndex].enPassant[0];
    this->enPassant[1] = this->moveHistory[moveIndex].enPassant[1];
    this->castlingFlags[0] = this->moveHistory[moveIndex].castlingFlags[0];
    this->castlingFlags[1] = this->moveHistory[moveIndex].castlingFlags[1];
    this->castlingFlags[2] = this->moveHistory[moveIndex].castlingFlags[2];
    this->castlingFlags[3] = this->moveHistory[moveIndex].castlingFlags[3];
    // revert the changes made by the move
    // pop the move and board from move history
    this->boardHistory.pop_back();
    this->moveHistory.pop_back();
    // /pop the move and board from history
    // TODO delete???
}


std::string Board::ToString() {
    std::map<int, std::string> pieceMap{
        {0, " "},
        {7, "k"}, {1,"K"},
        {8, "q"}, {2, "Q"},
        {9, "n"}, {3, "N"},
        {10, "b"}, {4, "B"},
        {11, "r"}, {5, "R"},
        {12, "p"}, {6, "P"}
    };
    std::string retString;
    retString += "\nking pos white: ";
    retString += this->ConvertPositionToStr(this->kingPos[0]);
    retString += ", king pos black: ";
    retString += this->ConvertPositionToStr(this->kingPos[1]);
    retString += ", 75 move rule counter: ";
    retString += this->seventyFiveMoveRuleCounter;
    retString += ", enPassant: ";
    retString += this->ConvertPositionToStr(this->enPassant);
    retString += "\n";
    retString += "\n-------------------------------\n";
    std::map<int, char> rowMap{ {0,'8'},{1,'7'},{2,'6'},{3,'5'},{4,'4'},{5,'3'},{6,'2'},{7,'1'} };
    for (int i = 0; i < 8; i++) {
        retString += "| ";
        for (int j = 0; j < 8; j++) {
            retString += pieceMap[this->board[i][j]];
            retString += " | ";
        }
        retString += rowMap[i];
        retString += "\n";
        retString += "-------------------------------\n";
    }
    retString += "  a   b   c   d   e   f   g   h";
    return retString;
}
int* Board::ConvertStrToPosition(const char* pos) {
    std::map<char, int> columnMap {{'a',0},{'b',1},{'c',2},{'d',3},{'e',4},{'f',5},{'g',6},{'h',7}};
    std::map<char, int> rowMap{ {'8',0},{'7',1},{'6',2},{'5',3},{'4',4},{'3',5},{'2',6},{'1',7} };
    return new int[2]{rowMap[pos[1]], columnMap[pos[0]]};
}
std::string Board::ConvertPositionToStr(int pos[2]) {
    std::map<int, std::string> columnMap{ {-1, "-"}, {0,"a"},{1,"b"},{2,"c"},{3,"d"},{4,"e"},{5,"f"},{6,"g"},{7,"h"}};
    std::map<int, std::string> rowMap{ {-1, "-"}, {0,"8"},{1,"7"},{2,"6"},{3,"5"},{4,"4"},{5,"3"},{6,"2"},{7,"1"}};
    return columnMap[pos[1]] + rowMap[pos[0]];
}