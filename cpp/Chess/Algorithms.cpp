#include "Algorithms.h"
#include "Board.h"
#include "Move.h"
#include "SafeQueue.cpp"
#include "Coordinates.h"
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>
#include <string>


int Algorithms::Perft(Board *board, int depth)
{
	int legalMoveCount = 0;

	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
	for (const std::pair<const Coordinates, std::vector<Move>> &keyValuePair : currentLegalMoves)
	{
		for (const Move &move : keyValuePair.second)
		{
			if (depth == 1)
			{
				legalMoveCount++;
			}
			else
			{
				Board b{ *board };

				board->MakeMove(move);
				legalMoveCount += Perft(board, depth - 1);
				board->Pop();
				if (!(b == *board)) {
					std::cout << board->ToString();
					std::cout << b.ToString();
					std::cout << move.ToString();
					std::cout << (b == *board);
					std::cout << "fuck";
				}
			}
		}
	}

	return legalMoveCount;
}

void Algorithms::Worker(SafeQueue<Board *> &queue, std::atomic<int> &result, int depth)
{
	while (!queue.isEmpty())
	{
		Board *currentTask = queue.dequeue();
		result += Perft(currentTask, depth);
	}
}

int Algorithms::PerftStarter(Board *board, int depth)
{
	SafeQueue<Board *> tasks;
	std::atomic<int> result = 0;

	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
	for (const std::pair<const Coordinates, std::vector<Move>> &keyValuePair : currentLegalMoves)
	{
		for (const auto &move : keyValuePair.second)
		{
			board->MakeMove(move);
			Board *newBoard = new Board(*(board));
			tasks.enqueue(newBoard);
			board->Pop();
		}
	}
	int threadCount = std::thread::hardware_concurrency() -1;
	std::vector<std::thread> threads;
	for (int i = 0; i < threadCount; i++)
	{
		std::thread t(Algorithms::Worker, std::ref(tasks), std::ref(result), depth);
		threads.push_back(std::move(t));
	}
	for (int i = 0; i < threadCount; i++)
	{
		threads[i].join();
	}
	return result;
}

int Algorithms::PerftStarterSingleThread(Board *board, int depth) { return this->Perft(board, depth); }
