#include "PieceCharacteristics.h"

#include "Coordinates.h"
#include "Board.h"
//#include <bits/stdc++.h>
#include <iterator>
#include <sstream>
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
        this->enPassant = ConvertStrToPosition(std::array<char, 2>{splitFen[3].c_str()[0], splitFen[3].c_str()[1]});
    }
    // /parse en passant destination

    // parse 75 move rule
    this->seventyFiveMoveRuleCounter = atoi(splitFen[4].c_str());
    // /parse 75 move rule

    // parse turn counter
    this->turnCounter = atoi(splitFen[5].c_str());
    // /parse turn counter
}

void Board::Capture(Coordinates destination)
{
    int capturedPiece = this->board[destination.row][destination.column];
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
        int flagIndex = destination.column == 0 ? (capturedPiece == 5 ? 1 : 3) : (capturedPiece == 5 ? 0 : 2);
        this->castlingFlags[flagIndex] = false;
    }
    // /take care of castling flags
}
void Board::MakeMove(const Move move)
{
    this->movesAreCalculated = false;

    int movingPiece = this->board[move.origin.row][move.origin.column];
    int capturedPiece = this->board[move.destination.row][move.destination.column];
    bool resetEnPassant = true;
    bool resetMoveCounter = false;

    // append a copy of this->board to board history
    std::array<std::array<int, 8>, 8> boardCopy = this->board;
    this->boardHistory.push_back(boardCopy);
    // /append a copy of this->board to board history

    // prepare variables for move history, probably temporary
    std::array<bool, 4> castlingFlags = this->castlingFlags;
    Coordinates enPassant{this->enPassant};
    int seventyFiveMoveRule = this->seventyFiveMoveRuleCounter;
    // /prepare variables for move history

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
            this->Capture(Coordinates(move.destination.row + (movingPiece == 6 ? 1 : -1), move.destination.column)); // capture the en passant victim
        }
        // /en passant
        // first move
        if ((move.origin.row == 1 && movingPiece == 12) ||
            (move.origin.row == 6 && movingPiece == 6))
        {
            this->enPassant.row = move.origin.row == 1 ? 2 : 5; // TODO: this could potentially be dangerous, if so, check if there is an adjacent enemy pawn
            this->enPassant.column = move.origin.column;        // TODO: this could potentially be dangerous, if so, check if there is an adjacent enemy pawn

            resetEnPassant = false;
        }
        // /first move
        // promotion
        if (move.promotion != 0)
        {
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
            this->board[move.origin.row][castleDestCol] = this->board[move.origin.row][castleFromCol];
            this->board[move.origin.row][castleFromCol] = 0;
        }
        // /castling
        // reset castle flags
        if (movingPiece == 1)
        {
            this->castlingFlags[0] = false;
            this->castlingFlags[1] = false;
        }
        else if (movingPiece == 7)
        {
            this->castlingFlags[2] = false;
            this->castlingFlags[3] = false;
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
        if (this->castlingFlags[0] && move.origin.row == 7 && move.origin.column == 7)
        {
            this->castlingFlags[0] = false;
        } // reset wk castle flag
        else if (this->castlingFlags[1] && move.origin.row == 7 && move.origin.column == 0)
        {
            this->castlingFlags[1] = false;
        } // reset wq castle flag
        else if (this->castlingFlags[2] && move.origin.row == 0 && move.origin.column == 7)
        {
            this->castlingFlags[2] = false;
        } // reset bk castle flag
        else if (this->castlingFlags[3] && move.origin.row == 0 && move.origin.column == 0)
        {
            this->castlingFlags[3] = false;
        } // reset bq castle flag
    }
    // /move is made by a rook
    // move the piece
    this->board[move.destination.row][move.destination.column] = this->board[move.origin.row][move.origin.column];
    this->board[move.origin.row][move.origin.column] = 0;
    // /move the piece
    // save the move in move history
    this->moveHistory.push_back(Move{move.origin, move.destination, move.promotion, movingPiece,capturedPiece, castlingFlags, enPassant, seventyFiveMoveRule});
    // /save the move in move history
    if (!sideToMove)
    {
        this->turnCounter++;
    }
    this->sideToMove = !this->sideToMove;
    // reset en passant flag and 75 move rule counter
    if (resetEnPassant)
    {
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
    // /reset en passant flag and 75 move rule counter
}
void Board::Pop()
{
    // TODO: revert attacked fields?
    if (this->boardHistory.size() > 0 && this->moveHistory.size() > 0)
    {
        this->movesAreCalculated = false;
        // revert the changes made by the move
        int boardIndex = this->boardHistory.size() - 1;
        int moveIndex = this->moveHistory.size() - 1;
        /*for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                this->board[i][j] = this->boardHistory[boardIndex][i][j];
            }
        }*/
        // new
        Move move = this->moveHistory[moveIndex];

        if (move.enPassant &&
            move.movingPiece == (!this->sideToMove ? 12 : 6) &&
            move.destination == move.enPassant) { // move was an en passant
        
        }
        else {
            if (move.capturedPiece != 0) { // there was a capture
                this->board[move.destination.row][move.destination.column] = move.capturedPiece;
            }
            if ((move.movingPiece == 1 || move.movingPiece == 7) &&
                ((move.destination.column - move.origin.column == 2) || (move.destination.column - move.origin.column == -2))) { // move was a castle
                bool kingSideCastle = (move.origin.column - move.destination.column) == -2;
                int castleFromCol = move.origin.column + (kingSideCastle ? 3 : -4);
                int castleDestCol = move.origin.column + (kingSideCastle ? 1 : -1);
                this->board[move.origin.row][castleFromCol] = this->board[move.origin.row][castleDestCol];
                this->board[move.origin.row][castleDestCol] = 0;
            }
            this->board[move.origin.row][move.origin.column] = move.movingPiece;
        }
        // /new

        // update kingPos if the moving piece was a king
        if (this->moveHistory[moveIndex].movingPiece == 1 ||
            this->moveHistory[moveIndex].movingPiece == 7)
        {
            this->kingPos[this->moveHistory[moveIndex].movingPiece == 1 ? 0 : 1].row = this->moveHistory[moveIndex].origin.row;
            this->kingPos[this->moveHistory[moveIndex].movingPiece == 1 ? 0 : 1].column = this->moveHistory[moveIndex].origin.column;
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
        if (sideToMove)
        {
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

std::map<Coordinates, std::vector<Move>> Board::GetAllLegalMoves()
{
    if (!movesAreCalculated)
    {
        this->allLegalMoves.clear();
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
std::vector<Move> Board::CalculateLegalMovesForPiece(Coordinates origin, int movingPiece, bool movingPieceColor)
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
                        legalMoves.push_back(Move{origin, currentlyCalculatedPosition, 0});
                    } // /the destination field is neither attacked nor defended by the opponent
                }     // /the field is not occupied by allied piece
            }         // /normal moves
            // castling
            if ((this->castlingFlags[movingPieceColor] || this->castlingFlags[movingPieceColor + 1]) &&
                !(this->attackLines[!movingPieceColor].size() > 0))
            { // can't castle if the king is attacked
                // king side castle
                if ((!this->board[origin.row][origin.column + 1] && !this->attackedFields[!movingPieceColor][origin.row][origin.column + 1]) &&
                    (!this->board[origin.row][origin.column + 2] && !this->attackedFields[!movingPieceColor][origin.row][origin.column + 2]))
                { // the fields between the king and rook are unoccupied and not attacked
                    legalMoves.push_back(Move{origin, Coordinates{origin.row, origin.column + 2}, 0});
                }
                // /king side castle
                // queen side castle
                if (((this->board[origin.row][origin.column - 1] == 0) && !this->attackedFields[!movingPieceColor][origin.row][origin.column - 1]) &&
                    ((this->board[origin.row][origin.column - 2] == 0) && !this->attackedFields[!movingPieceColor][origin.row][origin.column - 2]) &&
                    (this->board[origin.row][origin.column - 3] == 0))
                { // the fields between the king and rook are unoccupied and not attacked
                    legalMoves.push_back(Move{origin, Coordinates{origin.row, origin.column - 2}, 0});
                }
                // /queen side castle
            }
            // /castling
        }
    } // /king
    else if (movingPiece == 6 || movingPiece == 12)
    { // pawn TODO: check pin and attack lines
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
                if (attackLine.find(destination) != attackLine.end())
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

void Board::PushMove(std::vector<Move>& legalMoves, const Coordinates& origin, const Coordinates& destination, bool promotion, bool movingPieceColor) {
    legalMoves.push_back(Move{ origin, destination, 0 });
    if (promotion) {
        if (destination.row == 0 || destination.row == 7)
        {
            legalMoves.push_back(Move{ origin, destination, movingPieceColor ? 8 : 2 });
            legalMoves.push_back(Move{ origin, destination, movingPieceColor ? 9 : 3 });
            legalMoves.push_back(Move{ origin, destination, movingPieceColor ? 10 : 4 });
            legalMoves.push_back(Move{ origin, destination, movingPieceColor ? 11 : 5 });
        }
    }
    
}

void Board::CalculateAttackFields()
{
    // for each piece calculate it's attack fields and if the piece is sliding, calculate pin lines
    for (int row = 0; row < 8; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            int movingPiece = this->board[row][column];
            if (movingPiece != 0)
            {
                PieceCharacteristics pieceCharacteristics = this->GetPieceMovement(movingPiece);
                bool movingPieceColor = movingPiece > 6;
                // pawn
                if (movingPiece == 6 || movingPiece == 12)
                { // TODO: enpassant
                    int attackedRow = row + (movingPieceColor ? 1 : -1);
                    int attackedCol1 = column + 1;
                    int attackedCol2 = column - 1;
                    if (attackedRow < 8 && attackedRow >= 0)
                    {
                        if (attackedCol1 < 8)
                            this->SetAttackedField(movingPieceColor, Coordinates(attackedRow, attackedCol1));
                        if (attackedCol2 >= 0)
                            this->SetAttackedField(movingPieceColor, Coordinates(attackedRow, attackedCol2));
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
                            column + pieceCharacteristics.pieceMovement[directionIndex].column};
                        // move can be made
                        if (this->FieldIsInBounds(currentlyCalculatedPosition))
                        {
                            this->SetAttackedField(movingPieceColor, currentlyCalculatedPosition);
                            // pin line is the line from the center of the moving piece to the opponent's king
                            // piece in way is true if there is any piece in the way of the moving piece
                            // if the piece in way is of the same color as the moving piece, the pin line is broken
                            std::set<Coordinates> pinLine{Coordinates{row, column}, currentlyCalculatedPosition};
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
                                            else
                                                pinnedPiece = Coordinates(currentlyCalculatedPosition);
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
                                    this->pinLines[movingPieceColor].insert({pinnedPiece, pinLine});
                                else
                                {
                                    bool first = false;
                                    for (const Coordinates&field : pinLine)
                                    { // set attack fields along the attack line
                                        if (first)
                                            first = false;
                                        else
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
void Board::SetAttackedField(bool attackingPieceColor, Coordinates attackedField)
{
    // if attacked field is empty or attacked piece is opponent's -> the field is attacked, else the field is defended
    if ((this->board[attackedField.row][attackedField.column] > 0) &&
        (this->board[attackedField.row][attackedField.column] > 6) == attackingPieceColor)
        defendedFields[attackingPieceColor][attackedField.row][attackedField.column] = true;
    else
        attackedFields[attackingPieceColor][attackedField.row][attackedField.column] = true;
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
    retString += this->ConvertPositionToStr(this->kingPos[0]);
    retString += ", king pos black: ";
    retString += this->ConvertPositionToStr(this->kingPos[1]);
    retString += ", 75 move rule counter: ";
    retString += std::to_string(this->seventyFiveMoveRuleCounter);
    retString += ", enPassant: ";
    retString += this->ConvertPositionToStr(this->enPassant);
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
        retString += this->ConvertPositionToStr(keyValuePair.first) += ": ";
        for (const Move& move : keyValuePair.second) {
            retString += this->ConvertPositionToStr(move.destination);
            if (move.promotion != 0) {
                retString += "=" + pieceMap[move.promotion];
            }
            retString += ", ";
        }
        retString += "\n";
    }
    return retString;
}
Coordinates Board::ConvertStrToPosition(const std::array<char, 2> pos)
{
    std::map<char, int> columnMap{{'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4}, {'f', 5}, {'g', 6}, {'h', 7}};
    std::map<char, int> rowMap{{'8', 0}, {'7', 1}, {'6', 2}, {'5', 3}, {'4', 4}, {'3', 5}, {'2', 6}, {'1', 7}};
    return Coordinates(rowMap[pos[1]], columnMap[pos[0]]);
}
std::string Board::ConvertPositionToStr(Coordinates pos)
{
    std::map<int, std::string> columnMap{{-1, "-"}, {0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}, {4, "e"}, {5, "f"}, {6, "g"}, {7, "h"}};
    std::map<int, std::string> rowMap{{-1, "-"}, {0, "8"}, {1, "7"}, {2, "6"}, {3, "5"}, {4, "4"}, {5, "3"}, {6, "2"}, {7, "1"}};
    return columnMap[pos.column] + rowMap[pos.row];
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
