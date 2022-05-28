#include "PieceCharacteristics.h"

#include "Coordinates.h"
#include "Board.h"
#include "Zobrist.h"

//#include <bits/stdc++.h>
#include <iterator>
#include <sstream>
#include <iostream>

#include <array>
#include <map>


Board::Board(std::string fen)
{
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            this->board[i][j] = 0;
        }
    }

    this->InitPieceMovement();

    std::map<char, int> pieceMap{
        {'k', 7},
        {'K', 1},
        {'q', 8},
        {'Q', 2},
        {'n', 9},
        {'N', 3},
        {'b', 10},
        {'B', 4},
        {'r', 11},
        {'R', 5},
        {'p', 12},
        {'P', 6},
    };
    std::map<char, int> castlingMap{
        {'k', 2},
        {'K', 0},
        {'q', 3},
        {'Q', 1},
    };
    std::istringstream iss(fen);
    std::istream_iterator<std::string> first(iss), last;
    std::vector<std::string> splitFen; // splitfen[0] - piece positions 1 -side to move, 2 - castling flags, 3 - enpassant destination, 4 - 75moverule, 5 - turn counter
    std::copy(first, last, std::back_inserter(splitFen));

    // parse pieces
    int row = 0, col = 0;
    for (char character : splitFen[0])
    {
        if (character != '/')
        {
            if (isdigit(character))
            {
                col += (int)(character)-48;
            }
            else if (isalpha(character))
            {
                this->board[row][col] = pieceMap[character];
                if (character == 'K')
                {
                    this->kingPos[0].row = row;
                    this->kingPos[0].column = col;
                }
                else if (character == 'k')
                {
                    this->kingPos[1].row = row;
                    this->kingPos[1].column = col;
                }
                col++;
            }
        }
        else
        {
            row++;
            col = 0;
        }
    }
    // /parse pieces

    // parse side to move
    this->sideToMove = splitFen[1] == "w" ? false : true;
    // /parse side to move

    // parse castling flags
    if (splitFen[2] == "-")
    {
        this->castlingFlags[0] = false;
        this->castlingFlags[1] = false;
        this->castlingFlags[2] = false;
        this->castlingFlags[3] = false;
    }
    else
    {
        for (const auto &flag : splitFen[2])
        {
            this->castlingFlags[castlingMap[flag]] = true;
        }
    }
    // /parse castling flags

    // parse en passant destination
    if (splitFen[3] != "-")
    {
        this->enPassant.row = Coordinates(splitFen[3]).row;
        this->enPassant.column = Coordinates(splitFen[3]).column;

    }
    // /parse en passant destination

    // parse 75 move rule
    this->seventyFiveMoveRuleCounter = atoi(splitFen[4].c_str());
    // /parse 75 move rule

    // parse turn counter
    this->turnCounter = atoi(splitFen[5].c_str());
    // /parse turn counter
    this->hash.Recalculate(this);
}

Board::Board(const Board& other)
{
    for (int row = 0;row < 8;row++)
        for (int column = 0; column < 8; column++)
            this->board[row][column] = other.board[row][column];
    for (int i = 0; i < 4; i++)
        this->castlingFlags[i] = other.castlingFlags[i];
    this->seventyFiveMoveRuleCounter = other.seventyFiveMoveRuleCounter;
    this->turnCounter = other.turnCounter;
    this->kingPos[0].row = other.kingPos[0].row;
    this->kingPos[0].column = other.kingPos[0].column;
    this->kingPos[1].row = other.kingPos[1].row;
    this->kingPos[1].column = other.kingPos[1].column;
    this->enPassant.row = other.enPassant.row;
    this->enPassant.column = other.enPassant.column;

    this->movesAreCalculated = false;
    this->sideToMove = other.sideToMove;
    this->hash = other.hash;
}

void Board::Capture(Coordinates destination)
{
    int capturedPiece = this->board[destination.row][destination.column];
    this->hash.TogglePieceSquare(capturedPiece, destination);
    // remove the piece from the board
    this->board[destination.row][destination.column] = 0;
    // /remove the piece from the board
    // reset the 75move rule counter
    this->seventyFiveMoveRuleCounter = 0;
    // /reset the 75move rule counter
    // take care of castling flags
    if ((destination.column == 0 || destination.column == 7) &&
        (capturedPiece == 5 || capturedPiece == 11))
    { // check if the captured piece is a rook and is on the 1st or 8th column
        if (this->castlingFlags[0] && capturedPiece == 5 && destination.row == 7 && destination.column == 7) {
            this->castlingFlags[0] = false; // reset wk castle flag
            this->hash.ToggleCastle(0);
        }
        else if (this->castlingFlags[1] && capturedPiece == 5 && destination.row == 7 && destination.column == 0) {
            this->castlingFlags[1] = false; // reset wq castle flag
            this->hash.ToggleCastle(1);
        }
        else if (this->castlingFlags[2] && capturedPiece == 11 && destination.row == 0 && destination.column == 7) {
            this->castlingFlags[2] = false; // reset bk castle flag
            this->hash.ToggleCastle(2);
        }
        else if (this->castlingFlags[3] && capturedPiece == 11 && destination.row == 0 && destination.column == 0) {
            this->castlingFlags[3] = false; // reset bq castle flag
            this->hash.ToggleCastle(3);
        }
    }
    // /take care of castling flags
}
void Board::MakeMove(const Move& move)
{
    this->movesAreCalculated = false;

    int movingPiece = this->board[move.origin.row][move.origin.column];
    int capturedPiece = this->board[move.destination.row][move.destination.column];
    bool resetEnPassant = true;
    bool resetMoveCounter = false;

    // append a copy of this->board to board history
    /*std::array<std::array<int, 8>, 8> boardCopy = this->board;
    this->boardHistory.push_back(boardCopy);*/
    // /append a copy of this->board to board history

    // prepare variables for move history, probably temporary
    std::array<bool, 4> castlingFlags = this->castlingFlags;
    Coordinates enPassant{ this->enPassant };
    int seventyFiveMoveRule = this->seventyFiveMoveRuleCounter;
    // /prepare variables for move history

    this->hash.TogglePieceSquare(movingPiece, move.origin);
    bool promotion = false;

    // move is a take
    if (this->board[move.destination.row][move.destination.column] != 0)
    {
        this->Capture(move.destination);
        resetMoveCounter = true;
    }
    // /move is a take
    // move is made by a pawn
    if (movingPiece == 6 || movingPiece == 12)
    {
        this->seventyFiveMoveRuleCounter = 0;
        // en passant
        if (move.destination.row == this->enPassant.row && move.destination.column == this->enPassant.column)
        {
            this->hash.TogglePieceSquare(movingPiece == 6 ? 12 : 6, Coordinates(move.destination.row + (movingPiece == 6 ? 1 : -1), move.destination.column));
            this->Capture(Coordinates(move.destination.row + (movingPiece == 6 ? 1 : -1), move.destination.column)); // capture the en passant victim
        }
        // /en passant
        // first move
        if (((move.origin.row - move.destination.row) == 2) || ((move.origin.row - move.destination.row) == -2))
        {
            if (this->enPassant.column != -1)
                this->hash.ToggleEnpassant(this->enPassant.column);
            Coordinates right = Coordinates{ move.destination.row, move.destination.column + 1 };
            Coordinates left = Coordinates{ move.destination.row, move.destination.column - 1 };
            bool enemyPawnLeft = this->FieldIsInBounds(left) && (this->board[left.row][left.column] == (movingPiece == 6 ? 12 : 6));
            bool enemyPawnRight = this->FieldIsInBounds(right) && (this->board[right.row][right.column] == (movingPiece == 6 ? 12 : 6));
            if (enemyPawnLeft || enemyPawnRight) { // there is at least one adjacent enemy pawn
                this->enPassant.row = move.origin.row == 1 ? 2 : 5;
                this->enPassant.column = move.origin.column;
                resetEnPassant = false;
                this->hash.ToggleEnpassant(this->enPassant.column);
            }// /there is at least one adjacent enemy pawn
        }
        // /first move
        // promotion
        if (move.promotion != 0)
        {
            promotion = true;
            this->hash.TogglePieceSquare(move.promotion, move.destination);

            this->board[move.origin.row][move.origin.column] = move.promotion;
        }
        // /promotion
        resetMoveCounter = true;
    }
    // /move is made by a pawn
    // move is made by a king
    else if (movingPiece == 1 || movingPiece == 7)
    {
        bool kingSideCastle = (move.origin.column - move.destination.column) == -2;
        bool queenSideCastle = (move.origin.column - move.destination.column) == 2;
        // castling
        if (kingSideCastle || queenSideCastle)
        {
            int castleFromCol = move.origin.column + (kingSideCastle ? 3 : -4);
            int castleDestCol = move.origin.column + (kingSideCastle ? 1 : -1);

            this->hash.TogglePieceSquare(this->board[move.origin.row][castleFromCol], Coordinates(move.origin.row, castleFromCol));
            this->hash.TogglePieceSquare(this->board[move.origin.row][castleFromCol], Coordinates(move.origin.row, castleDestCol));

            this->board[move.origin.row][castleDestCol] = this->board[move.origin.row][castleFromCol];
            this->board[move.origin.row][castleFromCol] = 0;
        }
        // /castling
        // reset castle flags
        if (movingPiece == 1)
        {
            if (this->castlingFlags[0]) {
                this->hash.ToggleCastle(0);
                this->castlingFlags[0] = false;
            }
            if (this->castlingFlags[1]) {
                this->hash.ToggleCastle(1);
                this->castlingFlags[1] = false;
            }
        }
        else if (movingPiece == 7)
        {
            if (this->castlingFlags[2]) {
                this->hash.ToggleCastle(2);
                this->castlingFlags[2] = false;
            }
            if (this->castlingFlags[3]) {
                this->hash.ToggleCastle(3);
                this->castlingFlags[3] = false;
            }
        }
        // /reset castle flags
        // update king position
        this->kingPos[movingPiece == 1 ? 0 : 1].row = move.destination.row;
        this->kingPos[movingPiece == 1 ? 0 : 1].column = move.destination.column;
        // /update king position
    }
    // /move is made by a king
    // move is made by a rook
    else if (movingPiece == 5 || movingPiece == 11)
    {
        if (this->castlingFlags[0] && movingPiece == 5 && move.origin.row == 7 && move.origin.column == 7) {
            this->castlingFlags[0] = false; // reset wk castle flag
            this->hash.ToggleCastle(0);
        }
        else if (this->castlingFlags[1] && movingPiece == 5 && move.origin.row == 7 && move.origin.column == 0) {
            this->castlingFlags[1] = false; // reset wq castle flag
            this->hash.ToggleCastle(1);
        }
        else if (this->castlingFlags[2] && movingPiece == 11 && move.origin.row == 0 && move.origin.column == 7){
            this->castlingFlags[2] = false; // reset bk castle flag
            this->hash.ToggleCastle(2);
        }
        else if (this->castlingFlags[3] && movingPiece == 11 && move.origin.row == 0 && move.origin.column == 0){
            this->castlingFlags[3] = false; // reset bq castle flag
            this->hash.ToggleCastle(3);
        }
    }
    // /move is made by a rook
    // move the piece
    this->board[move.destination.row][move.destination.column] = this->board[move.origin.row][move.origin.column];
    this->board[move.origin.row][move.origin.column] = 0;
    // /move the piece
    // save the move in move history
    this->moveHistory.push_back(Move{move.origin, move.destination, move.promotion, movingPiece, capturedPiece, castlingFlags, enPassant, seventyFiveMoveRule});
    // /save the move in move history
    if (!sideToMove)
    {
        this->turnCounter++;
    }
    this->sideToMove = !this->sideToMove;
    this->hash.ToggleSTM();
    // reset en passant flag and 75 move rule counter
    if (resetEnPassant)
    {
        if (this->enPassant.column != -1)
            this->hash.ToggleEnpassant(this->enPassant.column);

        this->enPassant.row = -1;
        this->enPassant.column = -1;
    }
    if (resetMoveCounter)
    {
        this->seventyFiveMoveRuleCounter = 0;
    }
    else
    {
        this->seventyFiveMoveRuleCounter++;
    }
    if (!promotion)
        this->hash.TogglePieceSquare(movingPiece, move.destination);

    // /reset en passant flag and 75 move rule counter
}
bool Board::operator==(const Board& other)
{
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            if (this->board[row][column] != other.board[row][column])
                return false;
        }
    }
    for (int i = 0; i < 4; i++)
        if (this->castlingFlags[i] != other.castlingFlags[i])
            return false;
    if (this->enPassant != other.enPassant)
        return false;
    if (this->seventyFiveMoveRuleCounter != other.seventyFiveMoveRuleCounter)
        return false;
    if (this->turnCounter != other.turnCounter)
        return false;
    if (this->sideToMove != other.sideToMove)
        return false;
    return true;
}
void Board::Pop()
{
    // TODO: revert attacked fields?
    if (this->moveHistory.size() > 0)
    {
        this->movesAreCalculated = false;
        // revert the changes made by the move
        //int boardIndex = this->boardHistory.size() - 1;
        int moveIndex = this->moveHistory.size() - 1;
        Move move = this->moveHistory[moveIndex];

        if (move.enPassant &&
            move.movingPiece == (!this->sideToMove ? 12 : 6) &&
            move.destination == move.enPassant) { // move was an en passant
            this->board[move.enPassant.row + (!this->sideToMove ? -1 : 1)][move.enPassant.column] = (!this->sideToMove ? 6 : 12);
            this->hash.TogglePieceSquare((!this->sideToMove ? 6 : 12), Coordinates(move.enPassant.row + (!this->sideToMove ? -1 : 1), move.enPassant.column));
        }
        else if ((move.movingPiece == 1 || move.movingPiece == 7) &&
                ((move.destination.column - move.origin.column == 2) || (move.destination.column - move.origin.column == -2))) { // move was a castle
                bool kingSideCastle = (move.origin.column - move.destination.column) == -2;
                int castleFromCol = move.origin.column + (kingSideCastle ? 3 : -4);
                int castleDestCol = move.origin.column + (kingSideCastle ? 1 : -1);
                this->hash.TogglePieceSquare(this->board[move.origin.row][castleDestCol], Coordinates(move.origin.row, castleDestCol));
                this->hash.TogglePieceSquare(this->board[move.origin.row][castleDestCol], Coordinates(move.origin.row, castleFromCol));
                this->board[move.origin.row][castleFromCol] = this->board[move.origin.row][castleDestCol];
                this->board[move.origin.row][castleDestCol] = 0;

        }
        this->hash.TogglePieceSquare(this->board[move.destination.row][move.destination.column], Coordinates(move.destination.row, move.destination.column));
        this->board[move.destination.row][move.destination.column] = move.capturedPiece;
        this->board[move.origin.row][move.origin.column] = move.movingPiece;
        this->hash.TogglePieceSquare(move.movingPiece, Coordinates(move.origin.row, move.origin.column));
        if (move.capturedPiece != 0)
            this->hash.TogglePieceSquare(move.capturedPiece, Coordinates(move.destination.row, move.destination.column));


        // update kingPos if the moving piece was a king
        if (this->moveHistory[moveIndex].movingPiece == 1 ||
            this->moveHistory[moveIndex].movingPiece == 7)
        {
            this->kingPos[this->moveHistory[moveIndex].movingPiece == 1 ? 0 : 1].row = this->moveHistory[moveIndex].origin.row;
            this->kingPos[this->moveHistory[moveIndex].movingPiece == 1 ? 0 : 1].column = this->moveHistory[moveIndex].origin.column;
        }
        this->seventyFiveMoveRuleCounter = this->moveHistory[moveIndex].seventyFiveMoveRule;

        if (this->enPassant.column != this->moveHistory[moveIndex].enPassant.column) {
            if (this->enPassant.column != -1)
                this->hash.ToggleEnpassant(this->enPassant.column);
            if (this->moveHistory[moveIndex].enPassant.column != -1)
                this->hash.ToggleEnpassant(this->moveHistory[moveIndex].enPassant.column);
        }
        this->enPassant.row = this->moveHistory[moveIndex].enPassant.row;
        this->enPassant.column = this->moveHistory[moveIndex].enPassant.column;

        for (int i = 0; i < 4; i++) {
            if (this->castlingFlags[i] != this->moveHistory[moveIndex].castlingFlags[i]) {
                this->hash.ToggleCastle(i);
                this->castlingFlags[i] = this->moveHistory[moveIndex].castlingFlags[i];
            }
        }
        // revert the changes made by the move
        // decrement turn
        if (sideToMove)
        {
            this->turnCounter--;
        }
        this->sideToMove = !this->sideToMove;
        // /decrement turn
        // pop the move and board from move history
        //this->boardHistory.pop_back();
        this->moveHistory.pop_back();
        this->hash.ToggleSTM();
        // /pop the move and board from history
    }
}

std::map<Coordinates, std::vector<Move>> Board::GetAllLegalMoves()
{
    if (!movesAreCalculated)
    {
        this->Clear();
        this->CalculateAttackFields();
        this->CalculateLegalMoves();
        this->movesAreCalculated = true;
    }
    return this->allLegalMoves;
}
void Board::CalculateLegalMoves()
{
    if (this->attackLines[!this->sideToMove].size() > 1)
    {
        // If there are > 1 attack lines (double check):
        // only the king can move
        std::vector<Move> kingMoves = CalculateLegalMovesForPiece(this->kingPos[sideToMove], this->sideToMove ? 7 : 1, this->sideToMove);
        if (kingMoves.size() > 0)
            this->allLegalMoves.insert({this->kingPos[sideToMove], kingMoves});
    }
    else
    {
        for (int row = 0; row < 8; row++)
        {
            for (int column = 0; column < 8; column++)
            {
                int movingPiece = this->board[row][column];
                if (movingPiece != 0)
                {
                    int movingPieceColor = movingPiece > 6;
                    if (movingPieceColor == this->sideToMove)
                    {
                        std::vector<Move> pieceMoves = CalculateLegalMovesForPiece(Coordinates(row, column), movingPiece, movingPieceColor);
                        if (pieceMoves.size() > 0)
                            this->allLegalMoves.insert({Coordinates(row, column), pieceMoves});
                    }
                }
            }
        }
    }
}
std::vector<Move> Board::CalculateLegalMovesForPiece(const Coordinates& origin, int movingPiece, bool movingPieceColor)
{
    std::vector<Move> legalMoves;
    PieceCharacteristics pieceCharacteristics = this->GetPieceMovement(movingPiece);

    bool pinned = this->pinLines[!this->sideToMove].count(origin) == 1;
    bool kingIsInCheck = this->attackLines[!this->sideToMove].size() == 1;
    std::set<Coordinates> pinLine;
    std::set<Coordinates> attackLine;
    if (pinned)
    {
        pinLine = this->pinLines[!this->sideToMove][origin];
    }
    if (kingIsInCheck)
    {
        attackLine = this->attackLines[!this->sideToMove][0];
    }

    if (movingPiece == 1 || movingPiece == 7)
    { // king
        for (int directionIndex = 0; directionIndex < pieceCharacteristics.pieceMovement.size(); directionIndex++)
        {
            Coordinates currentlyCalculatedPosition = origin;
            currentlyCalculatedPosition += pieceCharacteristics.pieceMovement[directionIndex];


            if (this->FieldIsInBounds(currentlyCalculatedPosition)) // normal moves
            {
                int attackedPiece = this->board[currentlyCalculatedPosition.row][currentlyCalculatedPosition.column];
                if (!(attackedPiece != 0 && attackedPiece > 6 == movingPieceColor))
                { // the field is not occupied by allied piece
                    if (!this->attackedFields[!movingPieceColor][currentlyCalculatedPosition.row][currentlyCalculatedPosition.column] &&
                        !this->defendedFields[!movingPieceColor][currentlyCalculatedPosition.row][currentlyCalculatedPosition.column])
                    { // the destination field is neither attacked nor defended by the opponent
                        this->PushMove(legalMoves, origin, currentlyCalculatedPosition, false, movingPieceColor);
                    } // /the destination field is neither attacked nor defended by the opponent
                }     // /the field is not occupied by allied piece
            }         // /normal moves
        }
        // castling
        bool canKingCastle = this->castlingFlags[movingPieceColor * 2];
        bool canQueenCastle = this->castlingFlags[movingPieceColor * 2 + 1];
        if ((canKingCastle || canQueenCastle) &&
            !(this->attackLines[!movingPieceColor].size() > 0))
        { // can't castle if the king is attacked
            // king side castle
            if (canKingCastle &&
                (!this->board[origin.row][origin.column + 1] && !this->attackedFields[!movingPieceColor][origin.row][origin.column + 1]) &&
                (!this->board[origin.row][origin.column + 2] && !this->attackedFields[!movingPieceColor][origin.row][origin.column + 2]))
            { // the fields between the king and rook are unoccupied and not attacked
                this->PushMove(legalMoves, origin, Coordinates{ origin.row, origin.column + 2 }, false, movingPieceColor);

            }
            // /king side castle
            // queen side castle
            if (canQueenCastle &&
                ((this->board[origin.row][origin.column - 1] == 0) && !this->attackedFields[!movingPieceColor][origin.row][origin.column - 1]) &&
                ((this->board[origin.row][origin.column - 2] == 0) && !this->attackedFields[!movingPieceColor][origin.row][origin.column - 2]) &&
                (this->board[origin.row][origin.column - 3] == 0))
            { // the fields between the king and rook are unoccupied and not attacked
                this->PushMove(legalMoves, origin, Coordinates{ origin.row, origin.column - 2 }, false, movingPieceColor);
            }
            // /queen side castle
        }
        // /castling
    } // /king
    else if (movingPiece == 6 || movingPiece == 12)
    { // pawn
        int attackedRow = origin.row + (movingPieceColor ? 1 : -1);
        int attackedCol1 = origin.column + 1;
        int attackedCol2 = origin.column - 1;
        if (attackedRow < 8 && attackedRow >= 0) // capturing diagonally + enpassant TODO: make this more elegant
        {
            if (attackedCol1 < 8)
                if ((this->board[attackedRow][attackedCol1] != 0 && (this->board[attackedRow][attackedCol1] > 6 != movingPieceColor)) ||
                    this->enPassant == Coordinates{attackedRow, attackedCol1})
                {
                    if (this->MoveIsLegal(origin, Coordinates{ attackedRow, attackedCol1 }, movingPiece, movingPieceColor, pinned, kingIsInCheck, pinLine, attackLine))
                        this->PushMove(legalMoves, origin, Coordinates{ attackedRow, attackedCol1 }, true, movingPieceColor);
                }
            if (attackedCol2 >= 0)
                if ((this->board[attackedRow][attackedCol2] != 0 && this->board[attackedRow][attackedCol2] > 6 != movingPieceColor) ||
                    this->enPassant == Coordinates{attackedRow, attackedCol2})
                {
                    if (this->MoveIsLegal(origin, Coordinates{ attackedRow, attackedCol2 }, movingPiece, movingPieceColor, pinned, kingIsInCheck, pinLine, attackLine))
                        this->PushMove(legalMoves, origin, Coordinates{ attackedRow, attackedCol2 }, true, movingPieceColor);
                }
        } // /capturing diagonally + enpassant
        // normal move
        if (this->board[attackedRow][origin.column] == 0)
        {
            if (this->MoveIsLegal(origin, Coordinates(attackedRow, origin.column), movingPiece, movingPieceColor, pinned, kingIsInCheck, pinLine, attackLine))
                this->PushMove(legalMoves, origin, Coordinates(attackedRow, origin.column), true, movingPieceColor);
            // first move
            if ((origin.row == 1 && movingPiece == 12) ||
                (origin.row == 6 && movingPiece == 6))
            {
                Coordinates destination{ origin.row + (movingPieceColor ? 2 : -2), origin.column };
                if (this->board[destination.row][destination.column] == 0)
                    if (this->MoveIsLegal(origin, destination, movingPiece, movingPieceColor, pinned, kingIsInCheck, pinLine, attackLine))
                        this->PushMove(legalMoves, origin, destination, false, movingPieceColor);
            }
            // /first move
        }
        // /normal move
        
    } // /pawn
    else
    { // every other piece
        for (int directionIndex = 0; directionIndex < pieceCharacteristics.pieceMovement.size(); directionIndex++)
        {
            Coordinates currentlyCalculatedPosition{
                origin.row + pieceCharacteristics.pieceMovement[directionIndex].row,
                origin.column + pieceCharacteristics.pieceMovement[directionIndex].column};
            // move can be made
            if (this->FieldIsInBounds(currentlyCalculatedPosition))
            {
                if (this->MoveIsLegal(origin, currentlyCalculatedPosition, movingPiece, movingPieceColor, pinned, kingIsInCheck, pinLine, attackLine))
                    this->PushMove(legalMoves, origin, currentlyCalculatedPosition, false);
                if (pieceCharacteristics.isSliding)
                { // piece is sliding (rook, bishop, queen)
                    bool interrupted = this->board[currentlyCalculatedPosition.row][currentlyCalculatedPosition.column] != 0;
                    currentlyCalculatedPosition += pieceCharacteristics.pieceMovement[directionIndex];
                    while (!interrupted && this->FieldIsInBounds(currentlyCalculatedPosition))
                    { 
                        if (this->MoveIsLegal(origin, currentlyCalculatedPosition, movingPiece, movingPieceColor, pinned, kingIsInCheck, pinLine, attackLine))
                            this->PushMove(legalMoves, origin, currentlyCalculatedPosition, false);
                        interrupted = this->board[currentlyCalculatedPosition.row][currentlyCalculatedPosition.column] != 0;
                        currentlyCalculatedPosition += pieceCharacteristics.pieceMovement[directionIndex];
                    }
                } // /piece is sliding
            }// /move can be made
        }
    } // /every other piece
    return legalMoves;
}

bool Board::MoveIsLegal(const Coordinates &origin, const Coordinates &destination, int movingPiece, bool movingPieceColor,
    bool pinned, bool kingIsInCheck,
    const std::set<Coordinates> &pinLine, const std::set<Coordinates> &attackLine) {
    int attackedPiece = this->board[destination.row][destination.column];
    bool pieceInWay = attackedPiece != 0;
    if ((movingPiece == 6 || movingPiece == 12) && destination == this->enPassant)
        if (!this->EnPassantIsLegal(origin, destination, movingPieceColor)) // check if enpassant is legal (example: "rnbq1bnr/pppp1ppp/8/8/1k2p1RP/4P2N/PPPP1PP1/RNBQKB2 w Q - 2 7" 7. d4)
            return false;
    if ((attackedPiece == 0) ||
        (attackedPiece != 0 && attackedPiece > 6 != movingPieceColor))
    {
        if (kingIsInCheck || pinned)
        { // there is an attack line or the moving piece is pinned
            if (kingIsInCheck && pinned) {
                if (attackLine.find(destination) != attackLine.end() &&
                    pinLine.find(destination) != pinLine.end())
                {
                    return true;
                }
            }
            else if (kingIsInCheck)
            { // there is an attack line; the piece can only move to block the attack line
                if ((attackLine.find(destination) != attackLine.end()) ||
                    ((movingPiece==6 || movingPiece==12) && // check if enpassant is legal (example: "8/8/3p4/1Pp4r/1KR2pk1/8/4P1P1/8 w - c6 0 3" 3. bxc6)
                        (destination == this->enPassant) &&
                        (attackLine.find(movingPieceColor ?
                            Coordinates{destination.row -1, destination.column} :
                            Coordinates{ destination.row + 1, destination.column}) != attackLine.end())))
                {
                    return true;
                }
            } // /there is an attack line
            else if (pinned)
            { // the moving piece is pinned; the piece can only move along the pinLine
                if (pinLine.find(destination) != pinLine.end())
                {
                    return true;
                }
            } // /the moving piece is pinned
        }// /there is an attack line or the moving piece is pinned
        else
        {
            return true;
        }
    }
    return false;
}

bool Board::EnPassantIsLegal(const Coordinates& origin, const Coordinates& destination, bool movingPieceColor) {
    // check if the pawn is pinned by a rook or a queen, example: "rnbq1bnr/pppp1ppp/8/8/1k2p1RP/4P2N/PPPP1PP1/RNBQKB2 w Q - 2 7" 7. d4
    if (origin.column == 0 || origin.column == 7 || (origin.column == 1 && destination.column == 0) || (origin.column == 6 && destination.column == 7))
        return true;
    bool interrupted = false;
    int currentPiece = this->board[origin.row][0];
    int pieceBeforePawn = currentPiece;
    int pieceAfterPawn = 0;
    int column = 1;
    
    while (column < 8 && !interrupted) {
        if (column != origin.column && column != destination.column) {
            currentPiece = this->board[origin.row][column];
            if (column < origin.column && currentPiece != 0)
                pieceBeforePawn = currentPiece;
            if (column > origin.column && currentPiece != 0) {
                pieceAfterPawn = currentPiece;
                interrupted = true;
            }
        }
        column++;
    }
    if (pieceBeforePawn == 0 || pieceAfterPawn == 0)
        return true;
    if (((pieceBeforePawn > 6) == movingPieceColor && (pieceBeforePawn == 1 || pieceBeforePawn == 7))
        || ((pieceAfterPawn > 6) == movingPieceColor && (pieceAfterPawn == 1 || pieceAfterPawn == 7))) // piece before or after moving pawn is an allied king
        if ((pieceBeforePawn > 6 != movingPieceColor) && (pieceBeforePawn == 2 || pieceBeforePawn == 5 || pieceBeforePawn == 8 || pieceBeforePawn == 11) || // piece before or after moving pawn is an enemy rook or queen
            (pieceAfterPawn > 6 != movingPieceColor) && (pieceAfterPawn == 2 || pieceAfterPawn == 5 || pieceAfterPawn == 8 || pieceAfterPawn == 11))
            return false;
    return true;
}

void Board::PushMove(std::vector<Move>& legalMoves, const Coordinates& origin, const Coordinates& destination, bool promotion, bool movingPieceColor) {
    int movingPiece = this->board[origin.row][origin.column];
    int attackedPiece = this->board[destination.row][destination.column];
    if (promotion && (destination.row == 0 || destination.row == 7)) {
        legalMoves.push_back(Move{ origin, destination, movingPieceColor ? 8 : 2, movingPiece, attackedPiece });
        legalMoves.push_back(Move{ origin, destination, movingPieceColor ? 9 : 3, movingPiece, attackedPiece });
        legalMoves.push_back(Move{ origin, destination, movingPieceColor ? 10 : 4, movingPiece, attackedPiece });
        legalMoves.push_back(Move{ origin, destination, movingPieceColor ? 11 : 5, movingPiece, attackedPiece });
    }
    else
        legalMoves.push_back(Move{ origin, destination, 0, movingPiece, attackedPiece });
}

void Board::CalculateAttackFields(bool bothSides)
{
    // for each piece calculate it's attack fields and if the piece is sliding, calculate pin lines
    for (int row = 0; row < 8; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            int movingPiece = this->board[row][column];
            if (movingPiece != 0)
            {
                bool movingPieceColor = movingPiece > 6;
                if (bothSides || (movingPieceColor == !this->sideToMove)) {
                    PieceCharacteristics pieceCharacteristics = this->GetPieceMovement(movingPiece);
                    // pawn
                    if (movingPiece == 6 || movingPiece == 12)
                    {
                        int attackedRow = row + (movingPieceColor ? 1 : -1);
                        int attackedCol1 = column + 1;
                        int attackedCol2 = column - 1;
                        if (attackedRow < 8 && attackedRow >= 0)
                        {
                            if (attackedCol1 < 8) {
                                this->SetAttackedField(movingPieceColor, Coordinates(attackedRow, attackedCol1));
                                if (this->board[attackedRow][attackedCol1] == (movingPieceColor ? 1 : 7))
                                    this->attackLines[movingPieceColor].push_back(std::set<Coordinates>{ Coordinates{ row, column }});
                            }
                            if (attackedCol2 >= 0) {
                                this->SetAttackedField(movingPieceColor, Coordinates(attackedRow, attackedCol2));
                                if (this->board[attackedRow][attackedCol2] == (movingPieceColor ? 1 : 7))
                                    this->attackLines[movingPieceColor].push_back(std::set<Coordinates>{ Coordinates{ row, column }});
                            }
                        }
                    }
                    // /pawn
                    // every other piece
                    else
                    {
                        for (int directionIndex = 0; directionIndex < pieceCharacteristics.pieceMovement.size(); directionIndex++)
                        {
                            Coordinates currentlyCalculatedPosition{
                                row + pieceCharacteristics.pieceMovement[directionIndex].row,
                                column + pieceCharacteristics.pieceMovement[directionIndex].column };
                            // move can be made
                            if (this->FieldIsInBounds(currentlyCalculatedPosition))
                            {
                                this->SetAttackedField(movingPieceColor, currentlyCalculatedPosition);
                                // pin line is the line from the center of the moving piece to the opponent's king
                                // piece in way is true if there is any piece in the way of the moving piece
                                // if the piece in way is of the same color as the moving piece, the pin line is broken
                                std::set<Coordinates> pinLine{ Coordinates{row, column}, currentlyCalculatedPosition };
                                int attackedPiece = this->board[currentlyCalculatedPosition.row][currentlyCalculatedPosition.column];
                                bool pieceInWay = attackedPiece != 0;
                                bool enemyKingInWay = (attackedPiece != 0) &&
                                    (attackedPiece == (movingPieceColor ? 1 : 7));

                                Coordinates pinnedPiece = (pieceInWay && !enemyKingInWay) ? Coordinates(currentlyCalculatedPosition) : Coordinates();
                                if (pieceCharacteristics.isSliding)
                                { // piece is sliding (rook, bishop, queen)
                                    bool interrupted = pieceInWay &&
                                        ((attackedPiece > 6) == movingPieceColor);

                                    currentlyCalculatedPosition += pieceCharacteristics.pieceMovement[directionIndex];

                                    while (!interrupted && this->FieldIsInBounds(currentlyCalculatedPosition))
                                    {
                                        attackedPiece = this->board[currentlyCalculatedPosition.row][currentlyCalculatedPosition.column];

                                        if (!pieceInWay || (pieceInWay && enemyKingInWay))
                                        {
                                            this->SetAttackedField(movingPieceColor, currentlyCalculatedPosition);
                                        }
                                        if (attackedPiece != 0)
                                        { // there is a piece in way
                                            if ((attackedPiece > 6) == movingPieceColor)
                                            { // the piece in way is of moving piece color
                                                interrupted = true;
                                            }
                                            else if (attackedPiece == (movingPieceColor ? 1 : 7))
                                            { // the piece in way is the enemy's king
                                                if (pieceInWay)
                                                {
                                                    enemyKingInWay = true;
                                                    interrupted = true;
                                                }
                                                else
                                                {
                                                    enemyKingInWay = true;
                                                    pieceInWay = true;
                                                }
                                            }
                                            else
                                            { // the piece in way is any other enemy piece
                                                if (pieceInWay)
                                                    interrupted = true;
                                                else {
                                                    pinnedPiece = currentlyCalculatedPosition;
                                                    /*pinnedPiece.row = currentlyCalculatedPosition.row;
                                                    pinnedPiece.column = currentlyCalculatedPosition.column;*/
                                                }
                                                pieceInWay = true;
                                            }
                                        } // /there is a piece in way
                                        if (!enemyKingInWay)
                                        {
                                            pinLine.insert(currentlyCalculatedPosition);
                                        }
                                        currentlyCalculatedPosition += pieceCharacteristics.pieceMovement[directionIndex];
                                    }
                                } // /piece is sliding
                                if (enemyKingInWay)
                                {
                                    if (pinnedPiece)
                                        this->pinLines[movingPieceColor].insert({ pinnedPiece, pinLine });
                                    else
                                    {
                                        for (const Coordinates& field : pinLine)
                                        { // set attack fields along the attack line
                                            if (field != Coordinates{ row,column })
                                                this->SetAttackedField(movingPieceColor, field);
                                        }
                                        this->attackLines[movingPieceColor].push_back(pinLine);
                                    }
                                }
                            } // /move can be made
                        }
                    } // /every other piece
                }
            }
        }
    }
}
void Board::SetAttackedField(bool attackingPieceColor, const Coordinates& attackedField)
{
    // if attacked field is empty or attacked piece is opponent's -> the field is attacked, else the field is defended
    if ((this->board[attackedField.row][attackedField.column] > 0) &&
        (this->board[attackedField.row][attackedField.column] > 6) == attackingPieceColor)
        defendedFields[attackingPieceColor][attackedField.row][attackedField.column] = true;
    else
        attackedFields[attackingPieceColor][attackedField.row][attackedField.column] = true;
}
void Board::Clear()
{
    this->allLegalMoves.clear();
    this->attackLines[0].clear();
    this->attackLines[1].clear();
    this->pinLines[0].clear();
    this->pinLines[1].clear();
    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            this->attackedFields[0][row][column] = false;
            this->attackedFields[1][row][column] = false;
            this->defendedFields[0][row][column] = false;
            this->defendedFields[1][row][column] = false;

        }
    }
}
std::string Board::ToString()
{
    std::map<int, std::string> pieceMap{
        {0, " "},
        {7, "k"},
        {1, "K"},
        {8, "q"},
        {2, "Q"},
        {9, "n"},
        {3, "N"},
        {10, "b"},
        {4, "B"},
        {11, "r"},
        {5, "R"},
        {12, "p"},
        {6, "P"}};
    std::map<int, std::string> castlingMap{{2, "k"}, {0, "K"}, {3, "q"}, {1, "Q"}};
    std::string retString;
    retString += "\nking pos white: ";
    retString += this->kingPos[0].ToString();
    retString += ", king pos black: ";
    retString += this->kingPos[1].ToString();
    retString += ", 75 move rule counter: ";
    retString += std::to_string(this->seventyFiveMoveRuleCounter);
    retString += ", enPassant: ";
    retString += this->enPassant.ToString();
    retString += ", castling flags: ";
    for (int i = 0; i < 4; i++)
    {
        if (this->castlingFlags[i])
            retString += castlingMap[i];
    }
    retString += "\n";
    retString += "\n-------------------------------\n";
    std::map<int, char> rowMap{{0, '8'}, {1, '7'}, {2, '6'}, {3, '5'}, {4, '4'}, {5, '3'}, {6, '2'}, {7, '1'}};
    for (int i = 0; i < 8; i++)
    {
        retString += "| ";
        for (int j = 0; j < 8; j++)
        {
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
std::string Board::AttackedFieldsToString()
{
    std::string retString;
    std::map<int, char> rowMap{{0, '8'}, {1, '7'}, {2, '6'}, {3, '5'}, {4, '4'}, {5, '3'}, {6, '2'}, {7, '1'}};
    retString += "\nattacked fields (X), defended fields (+) black\n";
    for (int i = 0; i < 8; i++)
    {
        retString += "| ";
        for (int j = 0; j < 8; j++)
        {
            if (this->attackedFields[1][i][j])
            {
                retString += "X";
            }
            else if (this->defendedFields[1][i][j])
            {
                retString += "+";
            }
            else
                retString += " ";
            retString += " | ";
        }
        retString += rowMap[i];
        retString += "\n";
        retString += "-------------------------------\n";
    }
    retString += "  a   b   c   d   e   f   g   h";
    retString += "\nattacked fields (X), defended fields (+) white\n";
    for (int i = 0; i < 8; i++)
    {
        retString += "| ";
        for (int j = 0; j < 8; j++)
        {
            if (this->attackedFields[0][i][j])
            {
                retString += "X";
            }
            else if (this->defendedFields[0][i][j])
            {
                retString += "+";
            }
            else
                retString += " ";
            retString += " | ";
        }
        retString += rowMap[i];
        retString += "\n";
        retString += "-------------------------------\n";
    }
    retString += "  a   b   c   d   e   f   g   h";
    return retString;
}
std::string Board::LegalMovesToString() {
    std::map<int, std::string> pieceMap{
        {0, " "},
        {7, "k"},
        {1, "K"},
        {8, "q"},
        {2, "Q"},
        {9, "n"},
        {3, "N"},
        {10, "b"},
        {4, "B"},
        {11, "r"},
        {5, "R"},
        {12, "p"},
        {6, "P"} };
    std::string retString = "\n";
    for (const std::pair<Coordinates, std::vector<Move>>& keyValuePair : this->allLegalMoves) {
        retString += Coordinates::ToString(keyValuePair.first) += ": ";
        for (const Move& move : keyValuePair.second) {
            retString += Coordinates::ToString(move.destination);
            if (move.promotion != 0) {
                retString += "=" + pieceMap[move.promotion];
            }
            retString += ", ";
        }
        retString += "\n";
    }
    return retString;
}
Board::~Board()
{
    this->moveHistory.clear();
    this->boardHistory.clear();
}
std::array<std::array<std::array<bool, 8>, 8>, 16> Board::GetNeuralNetworkRepresentation()
{
    std::array<std::array<std::array<bool, 8>, 8>, 16> retArray;

    for (int row = 0; row < 8; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            if (this->board[row][column] != 0)
            {
                retArray[this->board[row][column] - 1][row][column] = true;
            }
            retArray[12][row][column] = this->attackedFields[0][row][column];
            retArray[13][row][column] = this->attackedFields[1][row][column];
            retArray[14][row][column] = this->defendedFields[0][row][column];
            retArray[15][row][column] = this->defendedFields[1][row][column];
        }
    }
    return retArray;
}

void Board::InitPieceMovement()
{
    // directions
    Coordinates u{-1, 0};
    Coordinates ur{-1, 1};
    Coordinates r{0, 1};
    Coordinates dr{1, 1};
    Coordinates d{1, 0};
    Coordinates dl{1, -1};
    Coordinates l{0, -1};
    Coordinates ul{-1, -1};

    Coordinates uur{-2, 1};
    Coordinates rru{-1, 2};
    Coordinates rrd{1, 2};
    Coordinates ddr{2, 1};
    Coordinates ddl{2, -1};
    Coordinates lld{1, -2};
    Coordinates llu{-1, -2};
    Coordinates uul{-2, -1};
    // /directions

    std::vector<Coordinates> wkMovement{u, ur, r, dr, d, dl, l, ul};
    std::vector<Coordinates> wqMovement{u, ur, r, dr, d, dl, l, ul};
    std::vector<Coordinates> wnMovement{uur, rru, rrd, ddr, ddl, lld, llu, uul};
    std::vector<Coordinates> wbMovement{ur, dr, dl, ul};
    std::vector<Coordinates> wrMovement{u, r, d, l};
    std::vector<Coordinates> wpMovement{u};
    std::vector<Coordinates> bkMovement{u, ur, r, dr, d, dl, l, ul};
    std::vector<Coordinates> bqMovement{u, ur, r, dr, d, dl, l, ul};
    std::vector<Coordinates> bnMovement{uur, rru, rrd, ddr, ddl, lld, llu, uul};
    std::vector<Coordinates> bbMovement{ur, dr, dl, ul};
    std::vector<Coordinates> brMovement{u, r, d, l};
    std::vector<Coordinates> bpMovement{d};

    PieceCharacteristics wk(wkMovement, false);
    PieceCharacteristics wq(wqMovement, true);
    PieceCharacteristics wn(wnMovement, false);
    PieceCharacteristics wb(wbMovement, true);
    PieceCharacteristics wr(wrMovement, true);
    PieceCharacteristics wp(wpMovement, false);
    PieceCharacteristics bk(bkMovement, false);
    PieceCharacteristics bq(bqMovement, true);
    PieceCharacteristics bn(bnMovement, false);
    PieceCharacteristics bb(bbMovement, true);
    PieceCharacteristics br(brMovement, true);
    PieceCharacteristics bp(bpMovement, false);

    this->pieceMovement[0] = wk;
    this->pieceMovement[1] = wq;
    this->pieceMovement[2] = wn;
    this->pieceMovement[3] = wb;
    this->pieceMovement[4] = wr;
    this->pieceMovement[5] = wp;
    this->pieceMovement[6] = bk;
    this->pieceMovement[7] = bq;
    this->pieceMovement[8] = bn;
    this->pieceMovement[9] = bb;
    this->pieceMovement[10] = br;
    this->pieceMovement[11] = bp;
}
PieceCharacteristics Board::GetPieceMovement(int piece)
{
    return this->pieceMovement[piece - 1];
}
