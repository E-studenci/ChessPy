#include "Algorithms.h"
#include "Board.h"
#include "Move.h"
#include "SafeQueue.cpp"
#include "Coordinates.h"
#include <thread>
#include <functional> 
#include <atomic>
Algorithms::Algorithms(std::string fen) {
	this->board = new Board(fen); 
}
int Algorithms::Perft(Board* board, int depth) {
	int legalMoveCount = 0;
	if (depth > 0) {
		std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
		for (const std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMoves) {
			for (const Move& move : keyValuePair.second) {
				if (depth == 1) {
					legalMoveCount++;
				}
				else {
					board->MakeMove(move);
					legalMoveCount += Perft(board, depth - 1);
					board->Pop();
				}
			}
		}
	}
	return legalMoveCount;
}

void Algorithms::Worker(SafeQueue<Board*>& queue, std::atomic<int>& result, int depth) {
	while (!queue.isEmpty()){
		Board* currentTask = queue.dequeue();
		result += Perft(currentTask, depth);
	}
}

int Algorithms::PerftStarter(int depth) {
	SafeQueue<Board*> tasks;
	std::atomic<int> result = 0;

	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
	for (const std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMoves) {
		for (const auto& move : keyValuePair.second) {
			this->board->MakeMove(move);
			Board* newBoard = new Board(*(this->board));
			tasks.enqueue(newBoard);
			this->board->Pop();
		}
	}
	int threadCount = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	for (int i = 0; i < threadCount; i++)
	{
		std::thread t(Algorithms::Worker, std::ref(tasks), std::ref(result), depth);
		threads.push_back(std::move(t));
	}
	for (int i = 0; i < threadCount; i++) {
		threads[i].join();
	}
	return result;
}

int Algorithms::PerftStarterSingleThread(int depth) { return this->Perft(new Board(*(this->board)), depth); }