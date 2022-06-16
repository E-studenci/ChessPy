#include "MoveOrderer.h"
#include "AlgorithmsConsts.h"
std::multiset<Move> MoveOrderer::OrderMoves(const Board& board, std::map<Coordinates, std::vector<Move>>& moves, bool hashedMove, uint16_t bestMoveHash, bool only_captures)
{
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
				if (hashedMove && (move.Hash() == bestMoveHash))
					move.score = 10000000;
				else
					move.score = this->MoveValue(board, move);
				result.insert(move);
			}
		}
	}
	return result;
}
double MoveOrdererHandcrafted::MoveValue(const Board& board, Move& move)
{
	double score = 0;
	int movingPiece = board.board[move.origin.row][move.origin.column];
	bool movingPieceColor = movingPiece > 6;
	movingPiece -= 1;
	int pieceScoreBefore =
		AlgorithmsConsts::POSITION_TABLE[movingPiece]
		[movingPieceColor ? (7 - move.origin.row) : move.origin.row][movingPieceColor ? (7 - move.origin.column) : move.origin.column]
		+ AlgorithmsConsts::PIECE_VALUE[movingPiece];
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
			AlgorithmsConsts::POSITION_TABLE[movingPiece]
			[movingPieceColor ? (7 - move.destination.row) : move.destination.row][movingPieceColor ? (7 - move.destination.column) : move.destination.column]
			+ AlgorithmsConsts::PIECE_VALUE[movingPiece];

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
		int capturedPieceValue = AlgorithmsConsts::POSITION_TABLE[capturedPiece]
			[(!movingPieceColor) ? (7 - move.destination.row) : move.destination.row][(!movingPieceColor) ? (7 - move.destination.column) : move.destination.column]
			+ AlgorithmsConsts::PIECE_VALUE[capturedPiece];
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
