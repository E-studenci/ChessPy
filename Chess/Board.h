#pragma once
#include "Move.h"
#include <string>
#include <vector>
#include <map>
class Board
{
public:
	int board[8][8]; // 0 - wk, 1 - wq, 2-wn, 3-wb, 4-wr, 5-wp, 6 - bk, 7 - bq, 8-bn, 9-bb, 10-br, 11-bp
	int kingPos[2]; // kingPos[0] - wk position, kingPos[1] - bk position
	int seventyFiveMoveRuleCounter; // moves without capture or pawn move
	bool castlingFlags[4]; // wk,wq,bk,bq
	int enPassant;
	bool sideToMove; // true - white, false - black
	bool attackFields[8][8], attackFieldsOpponent[8][8];
	int NeuralNetworkRepresentation[12][8][8];
	std::map<int, std::vector<Move>> allLegalMoves;
	std::map<int, std::vector<int>> pinLines, pinLinesOpponent;
	std::vector<Move> moveHistory;
	Board(int board[8][8], bool castling[4], int enPassant, bool sideToMove);
	Board(const Board& other);
	Board(std::string fen);
	Board() :Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
	std::vector<Move> CalculateLegalMoves() { return std::vector<Move>{}; }
	void Pop() {}
	void MakeMove(Move move) {}
	void ConvertForNeuralNetwork() {}
private:
	void CalculateAttackFields() {}
};
