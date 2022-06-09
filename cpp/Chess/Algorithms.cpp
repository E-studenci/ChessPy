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
				std::get<1>(result).push_back(std::tuple<Move, int>{ move, res });
			}
		}
	}
	return result;
}

int Algorithms::PerftStarterSingleThread(Board *board, int depth, bool divide) {
	this->count = 0;
	std::string retString;
	std::tuple<int, std::vector<std::tuple<Move, int>>> result = this->Perft(board, depth, divide);
	return std::get<0>(result);
	//retString += "Result: " + std::to_string(std::get<0>(result));
	//if (divide) {
	//	retString += "\nDivide: \n";
	//	for (int moveIndex = 0; moveIndex < std::get<1>(result).size()-1; moveIndex++) {
	//		retString += "\t";
	//		retString += std::get<0>(std::get<1>(result)[moveIndex]).ToString() + ": ";
	//		retString += std::to_string(std::get<1>(std::get<1>(result)[moveIndex]));
	//		retString += "\n";
	//	}
	//	retString += "\t";
	//	retString += std::get<0>(std::get<1>(result)[std::get<1>(result).size() - 1]).ToString() + ": ";
	//	retString += std::to_string(std::get<1>(std::get<1>(result)[std::get<1>(result).size() - 1]));
	//}
	//return retString;
}

EvaluationResult Algorithms::Root(Board* board, int max_depth, long timeInMillis, bool evaluatePosition, bool getOpponentBestMove)
{
	this->timer.Start(timeInMillis);
	int bestScore = 0;
	Move bestMove;
	int bestScoreOpponent = 0;
	Move bestMoveOpponent;
	int evaluation = 0;

	int reachedDepth = 0;
	this->count = 0;
	Board opponentBoard{ *board };
	opponentBoard.sideToMove = !opponentBoard.sideToMove;
	opponentBoard.hash.ToggleSTM();
	for (int depth = 1; depth <= max_depth;depth++) {
		reachedDepth++;

		// side to move best move
		int score;
		int alpha = MIN;
		int beta = MAX;
		int bestScoreCurrentDepth = 0;
		Move bestMoveCurrentDepth;
		if (this->timer.Poll(this->count)) {
			break; // Do not start a new ply
		}
		std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();
		for (const std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMoves)
		{
			for (const Move& move : keyValuePair.second)
			{
				//alpha = MIN;
				//beta = MAX;
				board->MakeMove(move);
				//score = -this->AlphaBeta(board, alpha, beta, depth);
				score = -this->AlphaBeta(board, -beta, -alpha, depth - 1);
				board->Pop();
				if (this->timer.Poll(this->count)) {
					break; // Discard
				}
				if (score > alpha) {
					alpha = score;
					bestScoreCurrentDepth = score;
					bestMoveCurrentDepth = move;
				}
			}
		}
		if (this->timer.Poll(this->count)) {
			reachedDepth--;
			break; // Discard
		}
		bestScore = bestScoreCurrentDepth;
		bestMove = bestMoveCurrentDepth;
		this->table.AddEntry(*board, EntryType::EXACT, bestScore, depth, bestMove);
		if (bestScore == MATE_SCORE)
			break;
		// /side to move best move
		// side to move evaluation
		if (evaluatePosition) {
			int evaluationScore = -this->AlphaBeta(board, -beta, -alpha, depth);
			if (this->timer.Poll(this->count)) {
				reachedDepth--;
				break; // Discard
			}
			evaluation = evaluationScore;
		}
		// /side to move evaluation
		// opponent best move
		if (getOpponentBestMove) {
			int scoreOpponent;
			int alphaOpponent = MIN;
			int betaOpponent = MAX;
			int bestScoreCurrentDepthOpponent = 0;
			Move bestMoveCurrentDepthOpponent;
			if (this->timer.Poll(this->count)) {
				break; // Do not start a new ply
			}
			std::map<Coordinates, std::vector<Move>> currentLegalMovesOpponent = opponentBoard.GetAllLegalMoves();
			for (const std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMovesOpponent)
			{
				for (const Move& move : keyValuePair.second)
				{
					//alpha = MIN;
					//beta = MAX;
					opponentBoard.MakeMove(move);
					//score = -this->AlphaBeta(board, alpha, beta, depth);
					scoreOpponent = -this->AlphaBeta(&opponentBoard, -betaOpponent, -alphaOpponent, depth - 1);
					opponentBoard.Pop();
					if (this->timer.Poll(this->count)) {
						break; // Discard
					}
					if (scoreOpponent > alphaOpponent) {
						alphaOpponent = scoreOpponent;
						bestScoreCurrentDepthOpponent = scoreOpponent;
						bestMoveCurrentDepthOpponent = move;
					}
				}
			}
			if (this->timer.Poll(this->count)) {
				reachedDepth--;
				break; // Discard
			}
			bestScoreOpponent = bestScoreCurrentDepthOpponent;
			bestMoveOpponent = bestMoveCurrentDepthOpponent;
			this->table.AddEntry(opponentBoard, EntryType::EXACT, bestScoreOpponent, depth, bestMoveOpponent);
		}
		// /opponent best move
		if (this->timer.Poll(this->count)) {
			reachedDepth--;
			break; // Discard
		}
	}
	return EvaluationResult(reachedDepth, bestMove, bestScore, evaluation * (board->sideToMove ? -1 : 1), bestMoveOpponent);
	//return std::pair<Move, std::pair<int, int>>{ best_move, std::pair<int, int>{best_score, reachedDepth} };
}

int Algorithms::EvaluatePosition(Board* board)
{
	if (board->GetAllLegalMoves().size() == 0) {
		if (board->KingInCheck())
			//return -MATE_SCORE * (board->sideToMove ? -1 : 1);
			return -MATE_SCORE;
		else
			return 0;
	}
	int score = this->EvalPieces(*board);
	//int doubledPawns = this->CountDoubledPawns(board);
	//int isolatedPawns = this->CountIsolatedPawns(board);
	//int blockedPawns = this->CountBlockedPawns(board);

	return score;// * (board->sideToMove? -1 : 1);
}

std::multiset<Move> Algorithms::OrderMoves(const Board& board, std::map<Coordinates, std::vector<Move>>& moves, bool hashedMove, uint16_t bestMoveHash, bool only_captures) {
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
				move.score = this->MoveValue(board, move, hashedMove, bestMoveHash);
				result.insert(move);
			}
		}
	}
	return result;
}

double Algorithms::MoveValue(const Board& board, Move& move,bool hashedMove, uint16_t bestMoveHash)
{
	if (hashedMove && (move.Hash() == bestMoveHash))
		return 1000000000;
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
			score = 10000003;
		else if (move.promotion == 3 || move.promotion == 9)
			score = 10000002;
		else if (move.promotion == 5 || move.promotion == 11)
			score = 10000001;
		else if (move.promotion == 4 || move.promotion == 10)
			score = 10000000;
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
		if (capturedPieceValue > pieceScoreBefore)
			move.goodCapture = true;
		//score += capture_value;
	}
	/*double ran = (((double)rand() * (0 - 0.05) / RAND_MAX) + 0);
	return score+ ran;*/
	return score;
}

int Algorithms::AlphaBeta(Board* board, int alpha, int beta, int depthLeft)
{
	if (this->timer.Poll(this->count)) {
		return 0;
	}
	if (board->ThreeFoldRepetition() || board->FifyMoveRuleDraw())
		return 0;
	bool found = false;
	bool foundHashedMove = false;

	uint16_t bestMoveHash = 0;
	Entry en = this->table.GetEntry(*board, &found);
	if (found && !(en.cutoff == EntryType::EMPTY_ENTRY)) {
		foundHashedMove = true;
		bestMoveHash = en.move_hash;
		if (en.depth >= depthLeft) {
			switch (en.cutoff) {
			case EntryType::LOWERBOUND:
				if (en.score > alpha) {
					alpha = en.score;
				}
				break;
			case EntryType::UPPERBOUND:
				if (en.score < beta) {
					beta = en.score;
					foundHashedMove = false;
				}
				break;
			case EntryType::EXACT:
				return en.score;
				break;
			}
			if (alpha >= beta)
				return en.score;
		}
	}
	if (depthLeft <= 0)
		//return this->EvaluatePosition(board);
		return this->Quiescence(board, alpha, beta);
	int origAlpha = alpha;
	int bestScore = MIN;
	Move bestMove = Move{};
	std::map<Coordinates, std::vector<Move>> currentLegalMoves = board->GetAllLegalMoves();

	//if (board->KingInCheck())
	//	depthLeft++;

	std::multiset<Move> currentLegalMovesSorted = this->OrderMoves(*board, currentLegalMoves, foundHashedMove, bestMoveHash);
	for (const Move& move : currentLegalMovesSorted){
		this->count++;
		board->MakeMove(move);
		int score = -AlphaBeta(board, -beta, -alpha, depthLeft - 1);
		board->Pop();
		if (this->timer.Poll(this->count)) {
			return 0;
		}
		if (score >= beta)
			return score;
		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
			if (score > alpha)
				alpha = score;
		}
	}
	if (this->timer.Poll(this->count)) {
		return 0;
	}
	if (currentLegalMovesSorted.size() == 0) {
		if (board->KingInCheck())
			return -MATE_SCORE;
		else return 0;
	}
	if (bestMove.origin)
		this->AddScoreToTable(*board, origAlpha, beta, bestScore, depthLeft, bestMove);
	return bestScore;
}

void Algorithms::AddScoreToTable(Board& board, int alphaOriginal, int beta, int score, int depth, Move& bestMove)
{
	if (score <= alphaOriginal)
		this->table.AddEntry(board, EntryType::UPPERBOUND, score, depth, bestMove);
	else if (score >= beta) 
		this->table.AddEntry(board, EntryType::LOWERBOUND, score, depth, bestMove);
	else 
		this->table.AddEntry(board, EntryType::EXACT, score, depth, bestMove);
}
int Algorithms::Quiescence(Board* board, int alpha, int beta, int ply)
{
	if (this->timer.Poll(this->count)) {
		return 0;
	}
	if (ply > this->max_depth)
		this->max_depth = ply;
	int stand_pat = this->EvaluatePosition(board);
	if (ply > MAX_PLY) {
		return stand_pat;
	}
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
	std::multiset<Move> currentLegalMovesSorted = this->OrderMoves(*board, currentLegalMoves, false, 0, true);
	//for (std::pair<const Coordinates, std::vector<Move>>& keyValuePair : currentLegalMoves)
	//{
	//	for (Move& move : keyValuePair.second)
	for (const Move& move : currentLegalMovesSorted) {
		if (!move.goodCapture)
			continue;
		this->count++;
		board->MakeMove(move);
		int score = -this->Quiescence(board, -beta, -alpha, ply +1);
		board->Pop();
		if (this->timer.Poll(this->count)) {
			return 0;
		}
		if (score >= beta)
			//return beta;
			return score;
		if (score > alpha)
			alpha = score;
	}
	if (this->timer.Poll(this->count)) {
		return 0;
	}
	return alpha;
}


int Algorithms::EvalPieces(const Board& board)
{
	int scores[2] = { 0, 0 };
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
	return (score * gamePhase + endScore * (24-gamePhase))/24;
}
