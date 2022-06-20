#pragma once
#include "Move.h"
#include "Board.h"
#include "SafeQueue.cpp"
#include "Evaluator.h"


#include <tuple>
#include <string>
#include <unordered_map>
#include <array>

#include <set>
#include "TranspositionTable.h"
#include "Timer.h"
#include "MoveOrderer.h"
#include "AlgorithmsConsts.h"
struct SearchResult{
	int evaluation = 0;
	int scoreAfterBestMove = 0;
	Move bestMove = Move();
	Move bestOpponentMove = Move();
	int reachedDepth = 0;
	std::vector<int> nodeCount = std::vector<int>();
	SearchResult() {
		this->bestMove = Move();
		this->scoreAfterBestMove = 0;
		this->evaluation = 0;
		this->bestOpponentMove = Move();
		this->reachedDepth = 0;
		this->nodeCount = std::vector<int>();
	}

	SearchResult(int reachedDepth, Move bestMove, int scoreAfterBestMove, int evaluation = 0, Move bestOpponentMove = Move(), std::vector<int> nodeCount=std::vector<int>()) {
		this->bestMove = bestMove;
		this->scoreAfterBestMove = scoreAfterBestMove;
		this->evaluation = evaluation;
		this->bestOpponentMove = bestOpponentMove;
		this->reachedDepth = reachedDepth;
		for (int depth = 0; depth < nodeCount.size(); depth++)
			this->nodeCount.push_back(nodeCount[depth]);
	}
};
struct AlphaBetaResult {
	int score = 0;
	int nodeCount = 0;
	AlphaBetaResult() {
		this->score = 0;
		this->nodeCount = 0;
	}
	AlphaBetaResult(int score, int nodeCount) {
		this->score = score;
		this->nodeCount = nodeCount;
	}
};
class SearchEngine{
public:
	SearchEngine() {
		this->table = TranspositionTable{};
		this->table.init();
		this->_moveOrderer = new MoveOrdererHandcrafted();
		this->_evaluator = new Evaluator();
		this->skipHashTables = false;
		for (int i = 0;i < 20;i++)
			for (int j = 0;j < this->killerMoveSize;j++)
				this->killerMoves[i][j] = uint16_t(0);
	}
	SearchEngine(int moveOrderer, EvaluatorParams evaluatorParams, bool skipHashTables) : SearchEngine(static_cast<MoveOrdererEnum>(moveOrderer), evaluatorParams, skipHashTables) {}
	SearchEngine(int moveOrderer, bool skipHashTables) : SearchEngine(moveOrderer, EvaluatorParams(), skipHashTables) {}
	SearchEngine(MoveOrdererEnum moveOrdererEnum, EvaluatorParams evaluatorParams, bool skipHashTables = false) {
		this->table = TranspositionTable{};
		this->table.init();
		switch (moveOrdererEnum) {
			case MoveOrdererEnum::HANDCRAFTED:
				this->_moveOrderer = new MoveOrdererHandcrafted();
				break;
			case MoveOrdererEnum::TRAINING:
				this->_moveOrderer = new MoveOrdererTraining();
				break;
		}
		this->_evaluator = new Evaluator(evaluatorParams);
		this->skipHashTables = skipHashTables;
		for (int i = 0;i < 20;i++)
			for (int j = 0;j < this->killerMoveSize;j++)
				this->killerMoves[i][j] = uint16_t(0);
	}
	int PerftStarterSingleThread(Board* board, int depth, bool divide = false);
	SearchResult Root(Board* board, int depth, long timeInMillis, bool evaluatePosition = false, bool getOpponentBestMove = false); // Returns the best move and score after the move

	AlphaBetaResult AlphaBeta(Board* board, int alpha, int beta, int depthLeft, int ply=-1);
	int count = 0;
	int max_depth = 0;
	TranspositionTable table;
private:
	const int killerMoveSize = 2;
	void insertKillerMove(uint16_t moveHash, int ply) {
		for (int slot = 0; slot < this->killerMoves[ply].size() - 1; slot++) {
			this->killerMoves[ply][slot] = this->killerMoves[ply][slot + 1];
		}
		this->killerMoves[ply][this->killerMoves[ply].size() - 1] = moveHash;
	}
	std::array<std::array<uint16_t, 2>, 20> killerMoves;
	bool skipHashTables = false;
	Timer timer;
	static std::tuple<int, std::vector<std::tuple<Move, int>>> Perft(Board* board, int depth, bool divide = false); // returns the number of moves possible
	AlphaBetaResult Quiescence(Board* board, int alpha, int beta, int ply=0);
	void AddScoreToTable(Board& board, int alphaOriginal, int beta, int score, int depth, Move& bestMove);

	int MAX_PLY = 100;

	MoveOrderer *_moveOrderer = nullptr;
	Evaluator* _evaluator = nullptr;
};
