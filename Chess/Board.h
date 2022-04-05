#pragma once
#include "Move.h"
#include <string>
#include <vector>
#include <map>
class Board
{
public:
	int board[8][8]; // [0,0] - a8, 1 - wk, 2 - wq, 3-wn, 4-wb, 5-wr, 6-wp, 7 - bk, 8 - bq, 9-bn, 10-bb, 11-br, 12-bp
	int kingPos[2][2]; // kingPos[0] - wk position, kingPos[1] - bk position
	int seventyFiveMoveRuleCounter; // moves without capture or pawn move
	int turnCounter; // turn counter
	bool castlingFlags[4]; // wk,wq,bk,bq
	int enPassant[2]; // en passant destination
	bool sideToMove; // true - white, false - black
	bool attackFields[8][8], attackFieldsOpponent[8][8]; // true if field is attacked, false if not
	int neuralNetworkRepresentation[14][8][8]; // 0-11 - pieces, 13,14 - attack fields
	std::map<int[2], std::vector<Move>> allLegalMoves; // piece_position: moves
	std::map<int[2], std::vector<int[2]>> pinLines, pinLinesOpponent; // key - pinned piece position, values - positions in the pinline
	std::vector<Move> moveHistory; // all past moves
	std::vector<int**> boardHistory; // all past positions for the threefold repetion rule
	Board(int board[8][8], bool castling[4], int enPassant, bool sideToMove);
	Board(const Board& other);
	Board(std::string fen);
	Board() :Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
	std::vector<Move> CalculateLegalMoves() { return std::vector<Move>{}; }
	void Pop(); // unmake the last move
	void MakeMove(Move move); // commit a move
	void ConvertForNeuralNetwork() {} // convert the board to its nn representation
	std::string ToString();
	static std::string ConvertPositionToStr(int pos[2]); // [0][0] to a8...
	static int* ConvertStrToPosition(const char pos[2]); // a8 to [0][0]...
private:
	void CalculateAttackFields(); // calculate attackfields for both sides
	void Capture(int destination[2]); // invoked inside Board::MakeMove if the move was a take
	int GameStatus(); // 0-ongoing, 1-draw, 2-win
};
