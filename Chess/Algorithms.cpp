#include "Algorithms.h"
#include "Board.h"
#include "Move.h"
#include "SafeQueue.cpp"

#include <thread>
#include <functional> 
Algorithms::Algorithms(std::string fen) {
	//this->board = new Board(fen); 
}
int Algorithms::Perft(Board* board, int depth) {
	int legalMoveCount = 0;
	//if (depth > 0) {
	//	board->CalculateLegalMoves();
	//	std::map<int[2], std::vector<Move>> currentLegalMoves = board->allLegalMoves;
	//	for (const std::pair<const int[2], std::vector<Move>>& keyValuePair : currentLegalMoves) {
	//		for (const Move& move : keyValuePair.second) {
	//			if (depth == 1) {
	//				legalMoveCount++;
	//			}
	//			else {
	//				board->MakeMove(&move);
	//				legalMoveCount += Perft(board, depth--);
	//				board->Pop();
	//			}
	//		}
	//	}
	//}
	return legalMoveCount;
}

void Algorithms::Worker(SafeQueue<Board*>& queue, std::atomic<int>& result, int depth) {
	//while (!queue.isEmpty()){
	//	Board* currentTask = queue.dequeue();
	//	result += Perft(currentTask, depth);
	//}
}

int Algorithms::PerftStarter(int depth) {
	//SafeQueue<Board*> tasks;
	std::atomic<int> result = 0;

	//this->board->CalculateLegalMoves();
	//for (const auto& keyValuePair : this->board->allLegalMoves) {
	//	for (const auto& move : keyValuePair.second) {
	//		this->board->MakeMove(&move);
	//		Board* newBoard = new Board(*(this->board));
	//		tasks.enqueue(newBoard);
	//		this->board->Pop();
	//	}
	//}
	//int threadCount = std::thread::hardware_concurrency();
	//std::vector<std::thread> threads;
	//for (int i = 0; i < threadCount; i++)
	//{
	//	std::thread t(Algorithms::Worker, std::ref(tasks), std::ref(result), depth);
	//	threads.push_back(std::move(t));
	//}
	//for (int i = 0; i < threadCount; i++) {
	//	threads[i].join();
	//}
	return result;
}