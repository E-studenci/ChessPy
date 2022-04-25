#pragma once
#include "Move.h"
#include "Board.h"
#include "SafeQueue.cpp"

#include<tuple>

class Algorithms
{
public:
	Algorithms(std::string fen);
	int PerftStarter(int depth); // used for multithreading
	int PerftStarterSingleThread(int depth);
	std::tuple<Move, float> GetBestMove(int depth, bool currentTurn); // Returns the best move and score after the move
	float EvaluatePosition(int depth, bool currentTurn); // Returns the position score
	~Algorithms() { delete this->board; }
private:
	Board* board;
	static int Perft(Board* board, int depth); // returns the number of moves possible
	static void Worker(SafeQueue<Board*>& queue, std::atomic<int>& result, int depth);
};

