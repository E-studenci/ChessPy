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
#include <sstream>
#include <map>
#include "Zobrist.h"

enum GameStatusEnum {
	ONGOING,
	STALEMATE,
	MATE,
	THREEFOLD_REPETITION,
	FIFTY_MOVER_RULE,
	INSUFFICIENT_MATERIAL
};

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
	std::vector<std::vector<int>> GetBoard();
	std::vector<Move> GetAllLegalMoves();
	int GetGameStatus();
	bool KingInCheck(bool opponent = false);
	void Pop(bool saveBoard=false);																			 // unmake the last move
	void MakeMove(const Move& move, bool saveBoard=false);														 // commit a move
	bool operator==(const Board& other);
	std::array<std::array<std::array<bool, 8>, 8>, 16> GetNeuralNetworkRepresentation(); // convert the board to its nn representation
	std::string ToString() const;
	std::string ToFen() const;
	std::string AttackedFieldsToString();
	std::string LegalMovesToString();
	bool ThreeFoldRepetition() {
		return std::count_if(this->boardHistory.begin(), this->boardHistory.end(),
			[&](uint64_t key) { return this->hash.Key() == key; }) >= 3;
	}
	bool FifyMoveRuleDraw() {
		return this->seventyFiveMoveRuleCounter > 99;
	}
	bool InsufficientMaterial();
	Zobrist hash;
	std::string GetCurrentKey() {
        std::ostringstream os;
        os << this->hash.Key();
        return os.str();
    }
	std::vector<std::string> fenHistory;
	Board* Clone();
	void ChangeSideToMove() {
		this->sideToMove = !this->sideToMove;
		this->hash.ToggleSTM();
	}
private:
	std::array<PieceCharacteristics, 12> pieceMovement;
	void InitPieceMovement();
	PieceCharacteristics GetPieceMovement(int piece);

	std::array<std::array<std::array<bool, 8>, 8>, 2> attackedFields; // 0-white, 1-black, 8x8bool table
	std::array<std::array<std::array<bool, 8>, 8>, 2> defendedFields; // 0-white, 1-black, 8x8bool table

	std::array<std::vector<std::array<Coordinates,8>>, 2> attackLines;		 // 0-white, 1-black
	std::array<std::map<Coordinates, std::array<Coordinates, 8>>, 2> pinLines; // 0-white, 1-black, the key is the pinned piece

	std::vector<uint64_t> boardHistory; // all past positions for the threefold repetion rule
	std::vector<Move> allLegalMoves;		 // piece_position: moves
	bool movesAreCalculated = false;

	void CalculateLegalMoves();
	std::vector<Move> CalculateLegalMovesForPiece(const Coordinates& origin, int movingPiece, bool movingPieceColor);
	bool MoveIsLegal(const Coordinates& origin, const Coordinates& destination, int movingPiece, bool movingPieceColor,
		bool pinned, bool kingIsInCheck,
		const std::array<Coordinates, 8>& pinLine, const std::array<Coordinates, 8>& attackLine);
	bool EnPassantIsLegal(const Coordinates& origin, const Coordinates& destination, bool movingPieceColor);
	void PushMove(std::vector<Move>& legalMoves, const Coordinates& origin, const Coordinates& destination,
		bool promotion = false, bool movingPieceColor = false);
	void CalculateAttackFields(bool bothSides=false);		   // calculate attackfields
	void Capture(Coordinates destination); // invoked inside Board::MakeMove if the move was a take
	bool PieceColor(int piece) { return piece < 7; }
	void SetAttackedField(bool attackingPieceColor, const Coordinates& attackedField);
	void Clear(); // clears attack fields etc
	inline bool FieldIsInBounds(const Coordinates& field)
	{
		return (field.row < 8 && field.row >= 0) && (field.column < 8 && field.column >= 0);
	}
	inline bool CoordinateInCollection(const Coordinates& coordinate, const std::vector<Coordinates> coordinateCollection) {
		for (int i = 0; i < coordinateCollection.size(); i++)
			if (coordinate == coordinateCollection[i])
				return true;
		return false;
	}
	inline bool CoordinateInCollection(const Coordinates& coordinate, const std::array<Coordinates,8> coordinateCollection) {
		for (int i = 0; i < coordinateCollection.size(); i++)
			if (coordinate == coordinateCollection[i])
				return true;
		return false;
	}
};
