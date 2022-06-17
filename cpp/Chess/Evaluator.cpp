#include "Evaluator.h"


int Evaluator::Evaluate(Board& board)
{
	if (board.GetAllLegalMoves().size() == 0) {
		if (board.KingInCheck())
			return -AlgorithmsConsts::MATE_SCORE;
		else
			return 0;
	}
	std::array<int,2> pieceScores{ 0, 0 };
	std::array<int, 2> piecePositionScores{ 0, 0 };
	std::array<int, 2> endGamePieceScores{ 0,0 };
	std::array<int, 2> endGamePiecePositionScores{ 0,0 };
	int gamePhase = 0;
	for (int row = 0; row < 8;row++) {
		for (int column = 0; column < 8; column++) {
			int piece = board.board[row][column];
			if (piece > 0) {
				bool pieceColor = piece > 6;
				int pieceIndex = piece - 1;
				gamePhase += AlgorithmsConsts::GAME_PHASE_SHIFT[pieceIndex];
				if (this->evalPieces) {
					pieceScores[pieceColor] += this->EvaluatePiece(board, pieceIndex, false);
					endGamePieceScores[pieceColor] += this->EvaluatePiece(board, pieceIndex, true);
				}
				if (this->evalPosition) {
					piecePositionScores[pieceColor] += this->EvaluatePiecePosition(board, pieceIndex, pieceColor, row, column, false);
					endGamePiecePositionScores[pieceColor] += this->EvaluatePiecePosition(board, pieceIndex, pieceColor, row, column, true);
				}
				if (this->evalPawns) {}
			}
		}
	}
	if (this->IsInsufficientMaterialDraw(pieceScores))
		return 0;

	int score = pieceScores[board.sideToMove] + piecePositionScores[board.sideToMove] 
		- pieceScores[!board.sideToMove] - piecePositionScores[!board.sideToMove];
	int endScore = endGamePieceScores[board.sideToMove] + endGamePiecePositionScores[board.sideToMove]
		- endGamePieceScores[!board.sideToMove] - endGamePiecePositionScores[!board.sideToMove];
	//std::cout << board.ToString();
	if (gamePhase > 24)
		gamePhase = 24;
	return (score * gamePhase + endScore * (24 - gamePhase)) / 24;
}
int Evaluator::EvaluatePiece(Board& board, int piece, bool endgame)
{
	if (endgame)
		return AlgorithmsConsts::END_GAME_PIECE_VALUE[piece];
	else 
		return AlgorithmsConsts::PIECE_VALUE[piece];
}

int Evaluator::EvaluatePiecePosition(Board& board, int piece, bool pieceColor, int row, int column, bool endgame)
{
	if (endgame)
		return AlgorithmsConsts::END_GAME_POSITION_TABLE[piece][pieceColor ? (7 - row) : row][pieceColor ? (7 - column) : column];
	else
		return AlgorithmsConsts::POSITION_TABLE[piece][pieceColor ? (7 - row) : row][pieceColor ? (7 - column) : column];
}

bool Evaluator::IsInsufficientMaterialDraw(std::array<int, 2>& pieceScores) {
	for (int colorIndex = 0; colorIndex < 2; colorIndex++) {
		bool color = colorIndex;
		if (pieceScores[color] == AlgorithmsConsts::KING_SCORE) {
			if (pieceScores[!color] == AlgorithmsConsts::KING_SCORE)
				return true;
			if (pieceScores[!color] == AlgorithmsConsts::KING_SCORE + AlgorithmsConsts::BISHOP_SCORE)
				return true;
			if (pieceScores[!color] == AlgorithmsConsts::KING_SCORE + AlgorithmsConsts::KNIGHT_SCORE)
				return true;
			if (pieceScores[!color] == AlgorithmsConsts::KING_SCORE + AlgorithmsConsts::KNIGHT_SCORE + AlgorithmsConsts::KNIGHT_SCORE)
				return true;
		}
	}
	return false;
}