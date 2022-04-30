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
#include <tuple>
#include <vector>




std::tuple<int, std::vector<std::tuple<Move, int>>> Algorithms::Perft(Board *board, int depth, bool divide)
{
	std::tuple<int, std::vector<std::tuple<Move, int>>> result; // result[0] - legalMoveCount, result[1] - legalMoveCount for move (used for divide)

	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
	for (const std::pair<const Coordinates, std::vector<Move>> &keyValuePair : currentLegalMoves)
	{
		for (const Move &move : keyValuePair.second)
		{
			int res = 0;
			if (depth == 1)
			{
				res = 1;
			}
			else
			{
				board->MakeMove(move);
				res = std::get<0>(Perft(board, depth - 1));
				board->Pop();
			}
			std::get<0>(result) += res;
			if (divide) {
				std::get<1>(result).push_back(std::tuple{ move, res });
			}
		}
	}
	return result;
}

void Algorithms::Worker(SafeQueue<Board *> &queue, std::atomic<int> &result, int depth)
{
	//while (!queue.isEmpty())
	//{
	//	Board *currentTask = queue.dequeue();
	//	result += Perft(currentTask, depth);
	//}
}

std::string Algorithms::PerftStarter(Board *board, int depth, bool divide)
{
	/*SafeQueue<Board *> tasks;
	std::atomic<int> result = 0;*/

	/*std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
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
	}*/
	return "";
}

std::string Algorithms::PerftStarterSingleThread(Board *board, int depth, bool divide) {
	std::string retString;
	std::tuple<int, std::vector<std::tuple<Move, int>>> result = this->Perft(board, depth, divide);
	retString += "Result: " + std::to_string(std::get<0>(result));
	if (divide) {
		retString += "\nDivide: \n";
		for (int moveIndex = 0; moveIndex < std::get<1>(result).size()-1; moveIndex++) {
			retString += "\t";
			retString += std::get<0>(std::get<1>(result)[moveIndex]).ToString() + ": ";
			retString += std::to_string(std::get<1>(std::get<1>(result)[moveIndex]));
			retString += "\n";
		}
		retString += "\t";
		retString += std::get<0>(std::get<1>(result)[std::get<1>(result).size() - 1]).ToString() + ": ";
		retString += std::to_string(std::get<1>(std::get<1>(result)[std::get<1>(result).size() - 1]));
	}
	return retString;
}
