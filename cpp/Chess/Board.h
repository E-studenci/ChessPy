#pragma once
#include "Move.h"
#include "Coordinates.h"
#include "AttackedLine.h"
#include "PieceCharacteristics.h"

#include <algorithm>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <set>
#include "Zobrist.h"
class Board
{
public:
	std::array<std::array<int, 8>, 8> board; // [0,0] - a8, 1 - wk, 2 - wq, 3-wn, 4-wb, 5-wr, 6-wp, 7 - bk, 8 - bq, 9-bn, 10-bb, 11-br, 12-bp
	std::array<Coordinates, 2> kingPos;		 // kingPos[0] - wk position, kingPos[1] - bk position
	int seventyFiveMoveRuleCounter;			 // moves without capture or pawn move
	int turnCounter;						 // turn counter
	std::array<bool, 4> castlingFlags;		 // wk,wq,bk,bq
	Coordinates enPassant;					 // en passant destination
	bool sideToMove;						 // false - white, true - black
	std::vector<Move> moveHistory;			 // all past moves
	Board(std::string fen);
	Board(const Board& other);
	Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
	~Board();
	std::map<Coordinates, std::vector<Move>> GetAllLegalMoves();
	bool KingInCheck(bool opponent=false) {
		if (opponent)
			return this->attackLines[this->sideToMove].size() > 0;
		return this->attackLines[!this->sideToMove].size()>0; }
	void Pop();																			 // unmake the last move
	void MakeMove(const Move& move);														 // commit a move
	bool operator==(const Board& other);
	std::array<std::array<std::array<bool, 8>, 8>, 16> GetNeuralNetworkRepresentation(); // convert the board to its nn representation
	std::string ToString() const;
	std::string AttackedFieldsToString();
	std::string LegalMovesToString();
	bool ThreeFoldRepetition() {
		return std::count_if(this->boardHistory.begin(), this->boardHistory.end(),
			[&](uint64_t key) { return this->hash.Key() == key; }) >= 3;
	}
	bool FifyMoveRuleDraw() {
		return this->seventyFiveMoveRuleCounter > 99;
	}
	Zobrist hash;

private:
	std::array<PieceCharacteristics, 12> pieceMovement;
	void InitPieceMovement();
	PieceCharacteristics GetPieceMovement(int piece);

	std::array<std::array<std::array<bool, 8>, 8>, 2> attackedFields; // 0-white, 1-black, 8x8bool table
	std::array<std::array<std::array<bool, 8>, 8>, 2> defendedFields; // 0-white, 1-black, 8x8bool table

	std::array<std::vector<std::set<Coordinates>>, 2> attackLines;		 // 0-white, 1-black
	std::array<std::map<Coordinates, std::set<Coordinates>>, 2> pinLines; // 0-white, 1-black, the key is the pinned piece

	std::vector<uint64_t> boardHistory; // all past positions for the threefold repetion rule
	std::map<Coordinates, std::vector<Move>> allLegalMoves;		 // piece_position: moves
	bool movesAreCalculated = false;

	void CalculateLegalMoves();
	std::vector<Move> CalculateLegalMovesForPiece(const Coordinates& origin, int movingPiece, bool movingPieceColor);
	bool MoveIsLegal(const Coordinates& origin, const Coordinates& destination, int movingPiece, bool movingPieceColor,
		bool pinned, bool kingIsInCheck,
		const std::set<Coordinates>& pinLine, const std::set<Coordinates>& attackLine);
	bool EnPassantIsLegal(const Coordinates& origin, const Coordinates& destination, bool movingPieceColor);
	void PushMove(std::vector<Move>& legalMoves, const Coordinates& origin, const Coordinates& destination,
		bool promotion = false, bool movingPieceColor = false);
	void CalculateAttackFields(bool bothSides=false);		   // calculate attackfields
	void Capture(Coordinates destination); // invoked inside Board::MakeMove if the move was a take
	int GameStatus();					   // 0-ongoing, 1-draw, 2-win
	bool PieceColor(int piece) { return piece < 7; }
	void SetAttackedField(bool attackingPieceColor, const Coordinates& attackedField);
	void Clear(); // clears attack fields etc
	inline bool FieldIsInBounds(const Coordinates& field)
	{
		return (field.row < 8 && field.row >= 0) && (field.column < 8 && field.column >= 0);
	}
};
