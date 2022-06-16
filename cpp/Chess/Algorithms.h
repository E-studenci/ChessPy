#pragma once
#include "Move.h"
#include "Board.h"
#include "SafeQueue.cpp"

#include <tuple>
#include <string>
#include <unordered_map>
#include <array>

#include <set>
#include "TranspositionTable.h"
#include "Timer.h"
#include "MoveOrderer.h"
#include "AlgorithmsConsts.h"
struct EvaluationResult
{
	int evaluation = 0;
	int scoreAfterBestMove = 0;
	Move bestMove = Move();
	Move bestOpponentMove = Move();
	int reachedDepth = 0;

	EvaluationResult() {
		this->bestMove = Move();
		this->scoreAfterBestMove = 0;
		this->evaluation = 0;
		this->bestOpponentMove = Move();
		this->reachedDepth = 0;
	}

	EvaluationResult(int reachedDepth, Move bestMove, int scoreAfterBestMove, int evaluation = 0, Move bestOpponentMove = Move()) {
		this->bestMove = bestMove;
		this->scoreAfterBestMove = scoreAfterBestMove;
		this->evaluation = evaluation;
		this->bestOpponentMove = bestOpponentMove;
		this->reachedDepth = reachedDepth;
	}
};
class Algorithms
{
public:
	Algorithms() {
		this->table = TranspositionTable{};
		this->table.init();
		this->_moveOrderer = new MoveOrdererHandcrafted();
	}

	Algorithms(MoveOrderer *moveOrderer) {
		this->table = TranspositionTable{};
		this->table.init();
		this->_moveOrderer = _moveOrderer;
	}
	int PerftStarterSingleThread(Board* board, int depth, bool divide = false);
	EvaluationResult Root(Board* board, int depth, long timeInMillis, bool evaluatePosition = false, bool getOpponentBestMove = false); // Returns the best move and score after the move

	int AlphaBeta(Board* board, int alpha, int beta, int depthLeft);
	int count = 0;
	int max_depth = 0;
	TranspositionTable table;
	MoveOrderer *_moveOrderer = nullptr;

private:
	Timer timer;
	static std::tuple<int, std::vector<std::tuple<Move, int>>> Perft(Board* board, int depth, bool divide = false); // returns the number of moves possible
	int EvaluatePosition(Board* board);	// Returns the position score
	int EvalPieces(const Board& board);
	int Quiescence(Board* board, int alpha, int beta, int ply=0);
	void AddScoreToTable(Board& board, int alphaOriginal, int beta, int score, int depth, Move& bestMove);
	int MAX_PLY = 100;
	
};
