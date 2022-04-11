#pragma once
#include "Move.h"
#include "Coordinates.h"
#include <string>
#include <vector>
#include <map>
class Board
{
public:
	int** board; // [0,0] - a8, 1 - wk, 2 - wq, 3-wn, 4-wb, 5-wr, 6-wp, 7 - bk, 8 - bq, 9-bn, 10-bb, 11-br, 12-bp
	Coordinates* kingPos; // kingPos[0] - wk position, kingPos[1] - bk position
	int seventyFiveMoveRuleCounter; // moves without capture or pawn move
	int turnCounter; // turn counter
	bool* castlingFlags; // wk,wq,bk,bq
	Coordinates enPassant; // en passant destination
	bool sideToMove; // false - white, true - black
	bool*** attackFields; // 0-white, 1-black, 8x8bool table
	int neuralNetworkRepresentation[14][8][8]; // 0-11 - pieces, 13,14 - attack fields
	std::map<Coordinates, std::vector<Move>> allLegalMoves; // piece_position: moves
	std::map<Coordinates, std::vector<Coordinates>> pinLines, pinLinesOpponent; // key - pinned piece position, values - positions in the pinline
	std::vector<Move*> moveHistory; // all past moves
	std::vector<int**> boardHistory; // all past positions for the threefold repetion rule TODO: make this a map
	Board(std::string fen);
	Board() :Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
	~Board();
	std::map<Coordinates, std::vector<Move>> CalculateLegalMoves();
	void Pop(); // unmake the last move
	void MakeMove(const Move* move); // commit a move
	void ConvertForNeuralNetwork() {} // convert the board to its nn representation
	std::string ToString();
	static std::string ConvertPositionToStr(Coordinates pos); // [0][0] to a8...
	static Coordinates ConvertStrToPosition(const char pos[2]); // a8 to [0][0]...
private:
	void CalculateAttackFields(); // calculate attackfields for both sides
	void Capture(Coordinates destination); // invoked inside Board::MakeMove if the move was a take
	int GameStatus(); // 0-ongoing, 1-draw, 2-win
	bool PieceColor(int piece) { return piece < 7; }
};
