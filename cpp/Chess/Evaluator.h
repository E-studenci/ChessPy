#pragma once

#include <functional>
#include <vector>
#include <array>
#include <set>

#include "AlgorithmsConsts.h"
#include "Move.h"
#include "Board.h"

class EvaluatorParams {
public:
	EvaluatorParams() :EvaluatorParams(false, false, false) {}
	EvaluatorParams(bool evalPieces, bool evalPosition, bool evalPawns) {
		this->evalPieces = evalPieces;
		this->evalPosition = evalPosition;
		this->evalPawns = evalPawns;
	}
	bool evalPieces;
	bool evalPosition;
	bool evalPawns;
};

class Evaluator
{
public:
	Evaluator() :Evaluator(EvaluatorParams{}) {}
	Evaluator(EvaluatorParams params) {
		this->evalPieces = params.evalPieces;
		this->evalPosition = params.evalPosition;
		this->evalPawns = params.evalPawns;
	}
	int Evaluate(Board& board);
	~Evaluator() = default;
private:
	int EvaluatePiece(Board& board, int piece, bool endgame);
	int EvaluatePiecePosition(Board& board, int piece, bool pieceColor, int row, int column, bool endgame);
	int EvaluatePawn(Board& board, int row, int column, bool color);
	bool IsInsufficientMaterialDraw(std::array<int, 2>& pieceScores);
	bool evalPieces;
	bool evalPosition;
	bool evalPawns;
};
