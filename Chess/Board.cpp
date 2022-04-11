#include "PieceMovement.h"
#include "Coordinates.h"
#include "Board.h"
#include <iterator>
#include <sstream>
#include <array>
#include <map>
Board::Board(std::string fen) {
    std::map<char, int> pieceMap
    { {'k',7},{'K',1},
        {'q',8},{'Q',2},
        {'n',9},{'N',3},
        {'b', 10}, {'B', 4},
        {'r', 11}, {'R', 5},
        {'p', 12}, {'P', 6},
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
                    this->kingPos[0].row = row;
                    this->kingPos[0].column = col;
                }
                else if (character == 'k') {
                    this->kingPos[1].row = row;
                    this->kingPos[1].column = col;
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
        this->enPassant = ConvertStrToPosition(splitFen[3].c_str());
    }
    // /parse en passant destination
    
    // parse 75 move rule
    this->seventyFiveMoveRuleCounter = atoi(splitFen[4].c_str());
    // /parse 75 move rule
    
    // parse turn counter
    this->turnCounter = atoi(splitFen[5].c_str());
    // /parse turn counter
}

void Board::Capture(Coordinates destination) {
    int capturedPiece = this->board[destination.row][destination.column];
    // remove the piece from the board
    this->board[destination.row][destination.column] = 0;
    // /remove the piece from the board
    // reset the 75move rule counter
    this->seventyFiveMoveRuleCounter = 0;
    // /reset the 75move rule counter
    // take care of castling flags
    if ((destination.column == 0 || destination.column == 7) &&
        (capturedPiece == 5 || capturedPiece == 11)) { // check if the captured piece is a rook and is on the 1st or 8th column
        int flagIndex = destination.column == 0 ? (capturedPiece == 5 ? 1 : 3) : (capturedPiece == 5 ? 0 : 2);
        this->castlingFlags[flagIndex] = false;
    }
    // /take care of castling flags
}
void Board::MakeMove(const Move move) {
    int movingPiece = this->board[move.from.row][move.from.column];
    bool resetEnPassant = true;
    bool resetMoveCounter = false;

    // append a copy of this->board to board history
    std::array<std::array<int,8>,8> boardCopy = this->board;
    this->boardHistory.push_back(boardCopy);
    // /append a copy of this->board to board history

    // prepare variables for move history, probably temporary
    std::array<bool, 4> castlingFlags = this->castlingFlags;
    Coordinates enPassant{ this->enPassant };
    int seventyFiveMoveRule = this->seventyFiveMoveRuleCounter;
    // /prepare variables for move history

    // move is a take
    if (this->board[move.destination.row][move.destination.column] != 0) {
        this->Capture(move.destination);
        resetMoveCounter = true;
    }
    // /move is a take
    // move is made by a pawn
    if (movingPiece == 6 || movingPiece == 12) {
        this->seventyFiveMoveRuleCounter = 0;
        // en passant
        if (move.destination.row == this->enPassant.row && move.destination.column == this->enPassant.column) {
            this->Capture(Coordinates(move.destination.row + (movingPiece == 6 ? 1 : -1), move.destination.column)); // capture the en passant victim
        }
        // /en passant
        // first move
        if ((move.from.row == 1 && movingPiece == 12) ||
            (move.from.row == 6 && movingPiece == 6)) {
            this->enPassant.row = move.from.row == 1 ? 2 : 5; // TODO: this could potentially be dangerous, if so, check if there is an adjacent enemy pawn
            this->enPassant.column = move.from.column; // TODO: this could potentially be dangerous, if so, check if there is an adjacent enemy pawn

            resetEnPassant = false;
        }
        // /first move
        // promotion
        if (move.promotion != 0) {
            this->board[move.from.row][move.from.column] = move.promotion;
        }
        // /promotion
        resetMoveCounter = true;
    }
    // /move is made by a pawn
    // move is made by a king TODO: update this->kingPos
    else if (movingPiece == 1 || movingPiece == 7) {
        bool kingSideCastle = move.from.column - move.destination.column == -2;
        bool queenSideCastle = move.from.column - move.destination.column == 2;
        // castling
        if (kingSideCastle || queenSideCastle) {
            int castleFromCol = move.from.column + kingSideCastle ? 3 : -4;
            int castleDestCol = move.from.column + kingSideCastle ? 1 : -1;
            this->board[move.from.row][castleDestCol] = this->board[move.from.row][castleFromCol];
            this->board[move.from.row][castleFromCol] = 0;
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
        this->kingPos[movingPiece == 1?0:1].row = move.destination.row;
        this->kingPos[movingPiece == 1?0:1].column = move.destination.column;
        // /update king position

    }
    // /move is made by a king
    // move is made by a rook
    else if (movingPiece == 5 || movingPiece == 11) {
        if (this->castlingFlags[0] && move.from.row == 7 && move.from.column == 7) { this->castlingFlags[0] = false; } // reset wk castle flag
        else if (this->castlingFlags[1] && move.from.row == 7 && move.from.column == 0) {this->castlingFlags[1] = false;} // reset wq castle flag
        else if (this->castlingFlags[2] && move.from.row == 0 && move.from.column == 7) {this->castlingFlags[2] = false;} // reset bk castle flag
        else if (this->castlingFlags[3] && move.from.row == 0 && move.from.column == 0) {this->castlingFlags[3] = false;} // reset bq castle flag
    }
    // /move is made by a rook
    // move the piece
    this->board[move.destination.row][move.destination.column] = this->board[move.from.row][move.from.column];
    this->board[move.from.row][move.from.column] = 0;
    // /move the piece
    // save the move in move history
    this->moveHistory.push_back(Move{move.from, move.destination, move.promotion, movingPiece, castlingFlags, enPassant, seventyFiveMoveRule});
    // /save the move in move history
    if (!sideToMove) {
        this->turnCounter++;
    }
    this->sideToMove = !this->sideToMove;
    // reset en passant flag and 75 move rule counter
    if (resetEnPassant) {
        this->enPassant.row = -1;
        this->enPassant.column = -1;
    }
    if (resetMoveCounter) {
        this->seventyFiveMoveRuleCounter = 0;
    }
    else {
        this->seventyFiveMoveRuleCounter++;
    }
    // /reset en passant flag and 75 move rule counter
}
void Board::Pop(){
    // TODO: revert attacked fields?
    if (this->boardHistory.size() > 0 && this->moveHistory.size() > 0) {
        // revert the changes made by the move
        int boardIndex = this->boardHistory.size() - 1;
        int moveIndex = this->moveHistory.size() - 1;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8;j++) {
                this->board[i][j] = this->boardHistory[boardIndex][i][j];
            }
        }
        // update kingPos if the moving piece was a king
        if (this->moveHistory[moveIndex].movingPiece == 1 ||
            this->moveHistory[moveIndex].movingPiece == 7) {
            this->kingPos[this->moveHistory[moveIndex].movingPiece == 1 ? 0 : 1].row = this->moveHistory[moveIndex].from.row;
            this->kingPos[this->moveHistory[moveIndex].movingPiece == 1 ? 0 : 1].column = this->moveHistory[moveIndex].from.column;
        }
        this->seventyFiveMoveRuleCounter = this->moveHistory[moveIndex].seventyFiveMoveRule;
        this->enPassant.row = this->moveHistory[moveIndex].enPassant.row;
        this->enPassant.column = this->moveHistory[moveIndex].enPassant.column;
        this->castlingFlags[0] = this->moveHistory[moveIndex].castlingFlags[0];
        this->castlingFlags[1] = this->moveHistory[moveIndex].castlingFlags[1];
        this->castlingFlags[2] = this->moveHistory[moveIndex].castlingFlags[2];
        this->castlingFlags[3] = this->moveHistory[moveIndex].castlingFlags[3];
        // revert the changes made by the move
        // decrement turn
        if (sideToMove) {
            this->turnCounter--;
        }
        this->sideToMove = !this->sideToMove;
        // /decrement turn
        // pop the move and board from move history
        this->boardHistory.pop_back();
        this->moveHistory.pop_back();
        // /pop the move and board from history
    }
}

std::map<Coordinates, std::vector<Move>> Board::CalculateLegalMoves() {
    this->CalculateAttackFields(); // temp ofc
    return std::map<Coordinates, std::vector<Move>>{};
}
void Board::CalculateAttackFields() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int piece = this->board[i][j];
            if (piece != 0) {
                bool pieceColor = piece > 6;
                // pawn
                if (piece == 6 || piece == 12) { // Omitting enPassant, since it's irrelevant for attacked fields, as such pawn only attack forward diagonal fields
                    int attackedRow = i + pieceColor ? 1 : -1;
                    int attackedCol1 = j + 1;
                    int attackedCol2 = j - 1;
                    if (attackedRow < 8 && attackedRow >= 0) {
                        if (attackedCol1 < 8)
                            this->attackFields[pieceColor][attackedRow][attackedCol1] = true;
                        if (attackedCol2 >= 0)
                            this->attackFields[pieceColor][attackedRow][attackedCol2] = true;
                    }
                }
                // /pawn
                // every other piece
                else {

                    if (PieceMovement::Get(piece).isSliding) {
                        
                    }
                }
                // /every other piece
            }
        }
    }
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
    std::map<int, std::string> castlingMap
    { {2,"k"},{0,"K"},
        {3, "q"},{1, "Q"}
    };
    std::string retString;
    retString += "\nking pos white: ";
    retString += this->ConvertPositionToStr(this->kingPos[0]);
    retString += ", king pos black: ";
    retString += this->ConvertPositionToStr(this->kingPos[1]);
    retString += ", 75 move rule counter: ";
    retString += std::to_string(this->seventyFiveMoveRuleCounter);
    retString += ", enPassant: ";
    retString += this->ConvertPositionToStr(this->enPassant);
    retString += ", castling flags: ";
    for (int i = 0; i < 4;i++) {
        if (this->castlingFlags[i])
            retString += castlingMap[i];
    }
    retString += "\n";
    retString += "\n-------------------------------\n";
    std::map<int, char> rowMap{ {0,'8'},{1,'7'},{2,'6'},{3,'5'},{4,'4'},{5,'3'},{6,'2'},{7,'1'} };
    for (int i = 0; i < 8; i++) {
        retString += "| ";
        for (int j = 0; j < 8; j++) {
            if (this->board[i][j] > 0) {
                retString += pieceMap[this->board[i][j]];
            }
            else retString += " ";
            retString += " | ";
        }
        retString += rowMap[i];
        retString += "\n";
        retString += "-------------------------------\n";
    }
    retString += "  a   b   c   d   e   f   g   h";
    return retString;
}
Coordinates Board::ConvertStrToPosition(const char* pos) {
    std::map<char, int> columnMap {{'a',0},{'b',1},{'c',2},{'d',3},{'e',4},{'f',5},{'g',6},{'h',7}};
    std::map<char, int> rowMap{ {'8',0},{'7',1},{'6',2},{'5',3},{'4',4},{'3',5},{'2',6},{'1',7} };
    return Coordinates(rowMap[pos[1]], columnMap[pos[0]]);
}
std::string Board::ConvertPositionToStr(Coordinates pos) {
    std::map<int, std::string> columnMap{ {-1, "-"}, {0,"a"},{1,"b"},{2,"c"},{3,"d"},{4,"e"},{5,"f"},{6,"g"},{7,"h"}};
    std::map<int, std::string> rowMap{ {-1, "-"}, {0,"8"},{1,"7"},{2,"6"},{3,"5"},{4,"4"},{5,"3"},{6,"2"},{7,"1"}};
    return columnMap[pos.column] + rowMap[pos.row];
}
Board::~Board() {
    this->moveHistory.clear();
    this->boardHistory.clear();
}