#pragma once
#include "Move.h"
#include "Board.h"
#include "SafeQueue.cpp"

#include <tuple>
#include <string>
class Algorithms
{
public:
	std::string PerftStarter(Board *board, int depth, bool divide=false); // used for multithreading
	std::string PerftStarterSingleThread(Board *board, int depth, bool divide = false);
	std::tuple<Move, float> GetBestMove(int depth, bool currentTurn); // Returns the best move and score after the move
	float EvaluatePosition(int depth, bool currentTurn);			  // Returns the position score

private:
	static std::tuple<int, std::vector<std::tuple<Move, int>>> Perft(Board *board, int depth, bool divide = false); // returns the number of moves possible
	static void Worker(SafeQueue<Board *> &queue, std::atomic<int> &result, int depth);
};
