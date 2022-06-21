#include "SearchEngine.h"
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
#include <functional>

std::tuple<int, std::vector<std::tuple<Move, int>>> SearchEngine::Perft(Board *board, int depth, bool divide)
{
	std::tuple<int, std::vector<std::tuple<Move, int>>> result; // result[0] - legalMoveCount, result[1] - legalMoveCount for move (used for divide)

	std::vector<Move> currentLegalMoves = board->GetAllLegalMoves();
	for (const Move &move : currentLegalMoves)
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
	return result;
}

int SearchEngine::PerftStarterSingleThread(Board *board, int depth, bool divide) {
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

SearchResult SearchEngine::Root(Board* board, int max_depth, long timeInMillis, bool evaluatePosition, bool getOpponentBestMove)
{
	for (int i = 0;i < 20;i++)
		for (int j = 0;j < this->killerMoveSize;j++)
			this->killerMoves[i][j] = uint16_t(0);
	this->timer.Start(timeInMillis);
	int bestScore = 0;
	Move bestMove;
	int bestScoreOpponent = 0;
	Move bestMoveOpponent;
	int evaluation = 0;

	std::vector<int> nodeCounts;
	int reachedDepth = 0;
	this->count = 0;
	Board opponentBoard{ *board };
	opponentBoard.sideToMove = !opponentBoard.sideToMove;
	opponentBoard.hash.ToggleSTM();
	for (int depth = 1; depth <= max_depth;depth++) {
		reachedDepth++;
		int nodeCount =0;
		// side to move best move
		int score;
		int alpha = AlgorithmsConsts::MIN;
		int beta = AlgorithmsConsts::MAX;
		int bestScoreCurrentDepth = 0;
		Move bestMoveCurrentDepth;
		if (this->timer.Poll(this->count)) {
			break; // Do not start a new ply
		}
		std::vector<Move> currentLegalMoves = board->GetAllLegalMoves();
		std::multiset<std::reference_wrapper<Move>> currentLegalMovesSorted = this->_moveOrderer->OrderMoves(*board, currentLegalMoves, false, 0, std::array<uint16_t,2>());
		for (const Move& move : currentLegalMovesSorted) {
			//alpha = MIN;
			//beta = MAX;
			board->MakeMove(move);
			//score = -this->AlphaBeta(board, alpha, beta, depth);
			auto result = this->AlphaBeta(board, -beta, -alpha, depth - 1);
			score = -result.score;
			nodeCount += result.nodeCount;
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
		if (this->timer.Poll(this->count)) {
			reachedDepth--;
			break; // Discard
		}
		bestScore = bestScoreCurrentDepth;
		bestMove = bestMoveCurrentDepth;
		this->table.AddEntry(*board, EntryType::EXACT, bestScore, depth-1, bestMove);
		if (bestScore == AlgorithmsConsts::MATE_SCORE)
			break;
		// /side to move best move
		// side to move evaluation
		if (evaluatePosition) {
			int evaluationScore = -this->AlphaBeta(board, -beta, -alpha, depth).score;
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
			int alphaOpponent = AlgorithmsConsts::MIN;
			int betaOpponent = AlgorithmsConsts::MAX;
			int bestScoreCurrentDepthOpponent = 0;
			Move bestMoveCurrentDepthOpponent;
			if (this->timer.Poll(this->count)) {
				break; // Do not start a new ply
			}
			std::vector<Move> currentLegalMovesOpponent = opponentBoard.GetAllLegalMoves();
			std::multiset<std::reference_wrapper<Move>> currentLegalMovesSorted = this->_moveOrderer->OrderMoves(*board, currentLegalMoves, false, 0, std::array<uint16_t, 2> ());
			for (const Move& move : currentLegalMovesSorted) {
				//alpha = MIN;
				//beta = MAX;
				opponentBoard.MakeMove(move);
				//score = -this->AlphaBeta(board, alpha, beta, depth);
				scoreOpponent = -this->AlphaBeta(&opponentBoard, -betaOpponent, -alphaOpponent, depth - 1).score;
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
			if (this->timer.Poll(this->count)) {
				reachedDepth--;
				break; // Discard
			}
			bestScoreOpponent = bestScoreCurrentDepthOpponent;
			bestMoveOpponent = bestMoveCurrentDepthOpponent;
			this->table.AddEntry(opponentBoard, EntryType::EXACT, bestScoreOpponent, depth-1, bestMoveOpponent);
		}
		// /opponent best move
		if (this->timer.Poll(this->count)) {
			reachedDepth--;
			break; // Discard
		}
		nodeCounts.push_back(nodeCount);
	}
	return SearchResult(reachedDepth, bestMove, bestScore, evaluation * (board->sideToMove ? 1 : -1), bestMoveOpponent, nodeCounts);
	//return std::pair<Move, std::pair<int, int>>{ best_move, std::pair<int, int>{best_score, reachedDepth} };
}

AlphaBetaResult SearchEngine::AlphaBeta(Board* board, int alpha, int beta, int depthLeft, int ply, bool isNullMove)
{
	ply++;
	int nodeCount = 1;
	if (this->timer.Poll(this->count)) {
		return AlphaBetaResult(0,nodeCount);
	}
	if (board->ThreeFoldRepetition() || board->FifyMoveRuleDraw())
		return AlphaBetaResult(0, nodeCount);
	if (depthLeft <= 0) {
		if (this->searchParams.useQuiescence) {
			auto result = this->Quiescence(board, alpha, beta);
			return AlphaBetaResult(result.score, nodeCount);
		}
		else return AlphaBetaResult(this->_evaluator->Evaluate(*board), nodeCount);
	}

	// Tranposition table
	bool found = false;
	bool foundHashedMove = false;

	uint16_t bestMoveHash = 0;
	Entry en = this->table.GetEntry(*board, &found);
	if (found && !(en.cutoff == EntryType::EMPTY_ENTRY)) {
		if (this->searchParams.useHashedMoves) {
			foundHashedMove = true;
			bestMoveHash = en.move_hash;
		}
		if (this->searchParams.useHashedPositions)
			if (en.depth >= depthLeft) {
				switch (en.cutoff) {
				case EntryType::LOWERBOUND:
					if (en.score > alpha) {
						alpha = en.score;
					}
					break;
				case EntryType::UPPERBOUND:
					foundHashedMove = false;
					if (en.score < beta) {
						beta = en.score;
					}
					break;
				case EntryType::EXACT:
					return AlphaBetaResult(en.score, nodeCount);
					break;
				}
				if (alpha >= beta)
					return AlphaBetaResult(en.score, nodeCount);
			}
	}
	// /Tranposition table

	std::vector<Move> currentLegalMoves = board->GetAllLegalMoves();
	// null move pruning
	if (this->searchParams.useNullMovePruning) {
		if (!isNullMove && !board->KingInCheck() && depthLeft > 1) {
			int pieceCount = 0;
			int pawnCount = 0;
			for (int row = 0; row < 8; row++) {
				for (int column = 0; column < 8;column++) {
					int piece = board->board[row][column];
					if (piece != 0 && ((piece > 6) == board->sideToMove)) {
						if (piece == 6 || piece == 12)
							pawnCount++;
						pieceCount++;
					}
				}
			}
			if (pieceCount > 3 && !(pieceCount == pawnCount + 1)) {
				board->ChangeSideToMove();
				int r = 2 + (depthLeft > (6 + ((pieceCount < 3) ? 2 : 0)));
				auto eval = AlphaBeta(board, -beta, -beta + 1, depthLeft - r - 1, ply, true);
				board->ChangeSideToMove();
				if (eval.score >= beta)
					return eval;
			}
		}
	}
	// /null move pruning

	int origAlpha = alpha;
	int bestScore = AlgorithmsConsts::MIN;
	Move bestMove = Move{};

	if (this->searchParams.useCheckExtension) {
		if (board->KingInCheck())
			depthLeft++;
	}

	std::multiset<std::reference_wrapper<Move>> currentLegalMovesSorted = this->_moveOrderer->OrderMoves(*board, currentLegalMoves, foundHashedMove, bestMoveHash, this->killerMoves[ply], false, this->searchParams.useMVVLVA);
	for (const Move& move : currentLegalMovesSorted){
		this->count++;
		board->MakeMove(move);
		auto result = AlphaBeta(board, -beta, -alpha, depthLeft - 1, ply);
		int score = -result.score;
		nodeCount += result.nodeCount;
		board->Pop();
		if (this->timer.Poll(this->count)) {
			return AlphaBetaResult(0, nodeCount);
		}
		if (score >= beta) {
			auto moveHash = move.Hash();
			if (this->searchParams.useKillerMoves) {
				if (!(foundHashedMove && en.move_hash == moveHash)
					&& (board->board[move.destination.row][move.destination.column] == 0))
					this->insertKillerMove(moveHash, ply);
			}
			return AlphaBetaResult(score, nodeCount);
		}
		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
			if (score > alpha)
				alpha = score;
		}
	}
	if (this->timer.Poll(this->count)) {
		return AlphaBetaResult(0, nodeCount);
	}
	if (currentLegalMovesSorted.size() == 0) {
		if (board->KingInCheck())
			return AlphaBetaResult(-AlgorithmsConsts::MATE_SCORE, nodeCount);
		else return AlphaBetaResult(0, nodeCount);
	}
	if (bestMove.origin)
		this->AddScoreToTable(*board, origAlpha, beta, bestScore, depthLeft, bestMove);
	return AlphaBetaResult(bestScore, nodeCount);
}

void SearchEngine::AddScoreToTable(Board& board, int alphaOriginal, int beta, int score, int depth, Move& bestMove)
{
	if (score <= alphaOriginal)
		this->table.AddEntry(board, EntryType::UPPERBOUND, score, depth, bestMove);
	else if (score >= beta) 
		this->table.AddEntry(board, EntryType::LOWERBOUND, score, depth, bestMove);
	else 
		this->table.AddEntry(board, EntryType::EXACT, score, depth, bestMove);
}
AlphaBetaResult SearchEngine::Quiescence(Board* board, int alpha, int beta, int ply){
	int nodeCount = 1;
	if (this->timer.Poll(this->count)) {
		return AlphaBetaResult(0, nodeCount);
	}
	if (ply > this->max_depth)
		this->max_depth = ply;
	int stand_pat = this->_evaluator->Evaluate(*board);
	if (ply > MAX_PLY) {
		return AlphaBetaResult(stand_pat, nodeCount);
	}
	if (stand_pat >= beta)
		return AlphaBetaResult(stand_pat, nodeCount);
	const int delta = AlgorithmsConsts::PIECE_VALUE[1];
	if (stand_pat < alpha - delta) {
		return AlphaBetaResult(alpha, nodeCount);
	}
	
	if (alpha < stand_pat)
		alpha = stand_pat;

	std::vector<Move> currentLegalMoves = board->GetAllLegalMoves();
	std::multiset<std::reference_wrapper<Move>> currentLegalMovesSorted = this->_moveOrderer->OrderMoves(*board, currentLegalMoves, false, 0, std::array<uint16_t, 2>(), true, this->searchParams.useMVVLVA);
	for (const Move& move : currentLegalMovesSorted) {
		this->count++;
		board->MakeMove(move);
		auto result = this->Quiescence(board, -beta, -alpha, ply + 1);
		int score = -result.score;
		nodeCount += result.nodeCount;
		board->Pop();
		if (this->timer.Poll(this->count)) {
			return AlphaBetaResult(0, nodeCount);
		}
		if (score >= beta)
			return AlphaBetaResult(score, nodeCount);
		if (score > alpha)
			alpha = score;
	}
	if (this->timer.Poll(this->count)) {
		return AlphaBetaResult(0,nodeCount);
	}
	return AlphaBetaResult(alpha,nodeCount);
}
