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
#include <limits>
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

std::pair<Move, double> Algorithms::GetBestMove(Board& board, int depth)
{
	std::pair<Move, double> bestMove{ Move{Coordinates{-1,-1}, Coordinates{-1,-1}}, std::numeric_limits<double>::infinity() * (board.sideToMove? 1:-1)};
	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board.GetAllLegalMoves();
	for (const std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMoves)
	{
		for (const Move& move : keyValuePair.second)
		{
			board.MakeMove(move);
			double moveScore = this->AlphaBeta(board, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), depth);
			board.Pop();
			if ((!board.sideToMove && (moveScore > bestMove.second)) || (board.sideToMove && (moveScore < bestMove.second))) {
				bestMove.first = move; // TODO: this probably doesn't work
				bestMove.second = moveScore;
			}
		}
	}
	return bestMove;
}

double Algorithms::EvaluatePosition(Board& board)
{
	if (board.GetAllLegalMoves().size() == 0) {
		if (board.KingInCheck())
			return -MATE_SCORE;
		else
			return 0;
	}
	double score = this->EvalPieces(board);
	//int doubledPawns = this->CountDoubledPawns(board);
	//int isolatedPawns = this->CountIsolatedPawns(board);
	//int blockedPawns = this->CountBlockedPawns(board);

	return score * (board.sideToMove? -1.0 : 1.0);
}

double Algorithms::AlphaBeta(Board& board, double alpha, double beta, int depthLeft)
{
	if (depthLeft <= 0) 
		return this->EvaluatePosition(board);
	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board.GetAllLegalMoves();
	for (const std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMoves)
	{
		for (const Move& move : keyValuePair.second)
		{
			board.MakeMove(move);
			double score = -AlphaBeta(board, -beta, -alpha, depthLeft - 1);
			board.Pop();
			if (score >= beta)
				return beta;
			if (score > alpha)
				alpha = score;
		}
	}
	return alpha;
}

double Algorithms::EvalPieces(const Board& board)
{
	int score = 0;
	int endGameScore = 0;
	int gamePhase = 0;
	for (int row = 0; row < 8;row++) {
		for (int column = 0; column < 8; column++) {
			int piece = board.board[row][column];
			if (piece > 0) {
				// evaluating piece and piece position
				piece -= 1;
				bool pieceColor = piece > 6;
				int pieceScore = 
					POSITION_TABLE[piece][pieceColor? (7-row): row][pieceColor ? (7 - column): column] + PIECE_VALUE[piece];
				int EndGamePieceScore = 
					END_GAME_POSITION_TABLE[piece][pieceColor ? (7 - row) : row][pieceColor ? (7 - column) : column] + END_GAME_PIECE_VALUE[piece];
				gamePhase += GAME_PHASE_SHIFT[piece];
				score += pieceScore * (pieceColor ? -1 : 1);
				endGameScore += EndGamePieceScore * (pieceColor ? -1 : 1);
				// /evaluating piece and piece position

			}
		}
	}
	if (gamePhase > 24)
		gamePhase = 24;
	return (score * gamePhase + endGameScore * (24-gamePhase))/24.0;
}
