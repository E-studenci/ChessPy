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
#include <set>


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
	this->count = 0;
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

std::pair<Move, double> Algorithms::GetBestMove(Board* board, int depth)
{
	this->count = 0;
	//std::pair<Move, double> bestMove{ Move{Coordinates{-1,-1}, Coordinates{-1,-1}}, std::numeric_limits<double>::infinity() * (board->sideToMove? -1:1)};
	std::pair<Move, double> bestMove{ Move{Coordinates{-1,-1}, Coordinates{-1,-1}}, -std::numeric_limits<double>::infinity() };

	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
	for (const std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMoves)
	{
		for (const Move& move : keyValuePair.second)
		{
			board->MakeMove(move);
			double moveScore = this->AlphaBeta(board, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), depth);
			board->Pop();
			//if (!board->sideToMove)
			//	moveScore *= -1;
			//if ((!board->sideToMove && (moveScore < bestMove.second)) || (board->sideToMove && (moveScore > bestMove.second))) {
			if (moveScore > bestMove.second) {
				bestMove.first = move; // TODO: this probably doesn't work
				bestMove.second = moveScore;
			}
		}
	}
	return bestMove;
}

std::pair<Move, double> Algorithms::Root(Board* board, int max_depth)
{
	double best_score = 0;
	Move best_move;
	for (int depth = 1; depth < max_depth; ++depth) {
		double score;
		double alpha = -std::numeric_limits<double>::infinity();
		double beta = std::numeric_limits<double>::infinity();
		std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
		for (const std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMoves)
		{
			for (const Move& move : keyValuePair.second)
			{
				board->MakeMove(move);
				score = -this->AlphaBeta(board, -beta, -alpha, depth - 1);
				board->Pop();

				if (score > alpha) {
					alpha = score;
					best_score = score;
					best_move = move;
				}
			}
		}
	}
	return std::pair<Move, double>{ best_move, best_score };
}

double Algorithms::EvaluatePosition(Board* board)
{
	if (board->GetAllLegalMoves().size() == 0) {
		if (board->KingInCheck())
			return -MATE_SCORE;
		else
			return 0;
	}
	double score = this->EvalPieces(*board);
	//int doubledPawns = this->CountDoubledPawns(board);
	//int isolatedPawns = this->CountIsolatedPawns(board);
	//int blockedPawns = this->CountBlockedPawns(board);

	return score * (board->sideToMove? -1.0 : 1.0);
}

std::multiset<Move> Algorithms::OrderMoves(const Board& board, std::map<Coordinates, std::vector<Move>>& moves, bool only_captures) {

	std::multiset<Move> result;
	for (std::pair<const Coordinates, std::vector<Move>>& keyValuePair : moves)
	{
		for (Move& move : keyValuePair.second)
		{
			if (!only_captures ||
				(board.board[move.destination.row][move.destination.column] != 0 ||
					(move.destination == board.enPassant &&
						(board.board[move.destination.row][move.destination.column] == 6 ||
							board.board[move.destination.row][move.destination.column] == 12)))) { // any move is accepted or the move is a capture
				move.score = this->MoveValue(board, move);
				result.insert(move);
			}
		}
	}
	return result;
}

double Algorithms::MoveValue(const Board& board, const Move& move)
{
	double score = 0;
	int movingPiece = board.board[move.origin.row][move.origin.column];
	bool movingPieceColor = movingPiece > 6;
	movingPiece -= 1;
	int pieceScoreBefore =
		POSITION_TABLE[movingPiece]
		[movingPieceColor ? (7 - move.origin.row) : move.origin.row][movingPieceColor ? (7 - move.origin.column) : move.origin.column]
		+ PIECE_VALUE[movingPiece];
	if (move.promotion != 0) {
		if (move.promotion == 2 || move.promotion == 8)
			score = 1000000003;
		else if (move.promotion == 3 || move.promotion == 9)
			score = 1000000002;
		else if (move.promotion == 5 || move.promotion == 11)
			score = 1000000001;
		else if (move.promotion == 4 || move.promotion == 10)
			score = 1000000000;
	}
	else {
		int pieceScoreAfter =
			POSITION_TABLE[movingPiece]
			[movingPieceColor ? (7 - move.destination.row) : move.destination.row][movingPieceColor ? (7 - move.destination.column) : move.destination.column]
			+ PIECE_VALUE[movingPiece];

		score = pieceScoreAfter - pieceScoreBefore;
	}

	int capturedPiece = board.board[move.destination.row][move.destination.column];
	if (capturedPiece != 0) {
		score += 2000;
		if (board.moveHistory.size() > 0)
		{
			if (board.moveHistory[board.moveHistory.size() - 1].destination == move.destination)
				score += 1000;
		}
		capturedPiece -= 1;
		int capturedPieceValue = POSITION_TABLE[capturedPiece]
			[(!movingPieceColor) ? (7 - move.destination.row) : move.destination.row][(!movingPieceColor) ? (7 - move.destination.column) : move.destination.column]
			+ PIECE_VALUE[capturedPiece];
		//double capture_value =fmin(0.0, capturedPieceValue - pieceScoreBefore);
		score += capturedPieceValue - pieceScoreBefore;
		//score += capture_value;
	}
	/*double ran = (((double)rand() * (0 - 0.05) / RAND_MAX) + 0);
	return score+ ran;*/
	return score;
}

double Algorithms::AlphaBeta(Board* board, double alpha, double beta, int depthLeft)
{
	if (depthLeft <= 0)
		//return this->EvaluatePosition(board);
		return this->Quiescence(board, alpha, beta);
	double bestScore = -std::numeric_limits<double>::infinity();
	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
	std::multiset<Move> currentLegalMovesSorted = this->OrderMoves(*board, currentLegalMoves);
	for (const Move& move : currentLegalMovesSorted){
		this->count++;
		board->MakeMove(move);
		double score = -AlphaBeta(board, -beta, -alpha, depthLeft - 1);
		board->Pop();

		if (score >= beta)
			return score;
		if (score > bestScore) {
			bestScore = score;
			if (score > alpha)
				alpha = score;
		}
	}
	//}
	return bestScore;
}

double Algorithms::Quiescence(Board* board, double alpha, double beta, int depth)
{
	if (depth > this->max_depth)
		this->max_depth = depth;
	double stand_pat = (board->sideToMove ? -1 : 1) * this->EvaluatePosition(board);
	if (stand_pat >= beta)
		//return beta;
		return stand_pat;
	const int delta = PIECE_VALUE[1];
	if (stand_pat < alpha - delta) {
		return alpha;
	}
	if (alpha < stand_pat)
		alpha = stand_pat;

	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
	std::multiset<Move> currentLegalMovesSorted = this->OrderMoves(*board, currentLegalMoves, true);
	//for (std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMoves)
	//{
	//	for (Move& move : keyValuePair.second)
	for (const Move& move : currentLegalMovesSorted) {
		this->count++;
		board->MakeMove(move);
		double score = -this->Quiescence(board, -beta, -alpha, depth+1);
		board->Pop();
		if (score >= beta)
			//return beta;
			return score;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}


double Algorithms::EvalPieces(const Board& board)
{
	int scores[2] = { 0 , 0};
	int endGameScores[2] = {0,0};
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
				scores[pieceColor] += pieceScore;
				//score += pieceScore * (pieceColor ? -1 : 1);
				endGameScores[pieceColor] += EndGamePieceScore;
				// /evaluating piece and piece position

			}
		}
	}
	int score = scores[board.sideToMove] - scores[!board.sideToMove];
	int endScore = endGameScores[board.sideToMove] - endGameScores[!board.sideToMove];
	//std::cout << board.ToString();
	if (gamePhase > 24)
		gamePhase = 24;
	return (score * gamePhase + endScore * (24-gamePhase))/24.0;
}
