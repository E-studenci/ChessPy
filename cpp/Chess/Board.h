#pragma once
#include "Move.h"
#include "Coordinates.h"
#include "AttackedLine.h"
#include "PieceCharacteristics.h"

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
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
	Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
	~Board();
	std::map<Coordinates, std::vector<Move>> GetAllLegalMoves();
	void Pop();																			 // unmake the last move
	void MakeMove(const Move move);														 // commit a move
	std::array<std::array<std::array<bool, 8>, 8>, 16> GetNeuralNetworkRepresentation(); // convert the board to its nn representation
	std::string ToString();
	std::string AttackedFieldsToString();
	static std::string ConvertPositionToStr(Coordinates pos);				// [0][0] to a8...
	static Coordinates ConvertStrToPosition(const std::array<char, 2> pos); // a8 to [0][0]...

	PieceCharacteristics GetPieceMovement(int piece);

private:
	std::array<PieceCharacteristics, 12> pieceMovement;
	void InitPieceMovement();

	std::array<std::array<std::array<bool, 8>, 8>, 2> attackedFields; // 0-white, 1-black, 8x8bool table
	std::array<std::array<std::array<bool, 8>, 8>, 2> defendedFields; // 0-white, 1-black, 8x8bool table

	std::array<std::vector<std::vector<Coordinates>>, 2> attackLines;		 // 0-white, 1-black
	std::array<std::map<Coordinates, std::vector<Coordinates>>, 2> pinLines; // 0-white, 1-black, the key is the pinned piece

	std::vector<std::array<std::array<int, 8>, 8>> boardHistory; // all past positions for the threefold repetion rule TODO: make this a map
	std::map<Coordinates, std::vector<Move>> allLegalMoves;		 // piece_position: moves
	bool movesAreCalculated;

	void CalculateLegalMoves();
	std::vector<Move> CalculateLegalMovesForPiece(Coordinates from, int movingPiece);
	void CalculateAttackFields();		   // calculate attackfields for both sides
	void Capture(Coordinates destination); // invoked inside Board::MakeMove if the move was a take
	int GameStatus();					   // 0-ongoing, 1-draw, 2-win
	bool PieceColor(int piece) { return piece < 7; }
	void SetAttackedField(bool attackingPieceColor, Coordinates attackedField);
	inline bool FieldIsInBounds(Coordinates field)
	{
		return (field.row < 8 && field.row >= 0) && (field.column < 8 && field.column >= 0);
	}
};
