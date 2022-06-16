#pragma once
#include <array>

class AlgorithmsConsts
{
public:
	static const std::array<std::array<int, 8>, 8> PAWN;
	static const std::array<std::array<int, 8>, 8> END_GAME_PAWN;
	static const std::array<std::array<int, 8>, 8> KNIGHT;
	static const std::array<std::array<int, 8>, 8> END_GAME_KNIGHT;
	static const std::array<std::array<int, 8>, 8> BISHOP;
	static const std::array<std::array<int, 8>, 8> END_GAME_BISHOP;
	static const std::array<std::array<int, 8>, 8> ROOK;
	static const std::array<std::array<int, 8>, 8> END_GAME_ROOK;
	static const std::array<std::array<int, 8>, 8> QUEEN;
	static const std::array<std::array<int, 8>, 8> END_GAME_QUEEN;
	static const std::array<std::array<int, 8>, 8> KING;
	static const std::array<std::array<int, 8>, 8> END_GAME_KING;
	static const std::array<int, 12> PIECE_VALUE;
	static const std::array<int, 12> END_GAME_PIECE_VALUE;
	static const std::array<int, 12> GAME_PHASE_SHIFT;
	static const std::array<std::array<std::array<int, 8>, 8>, 12> POSITION_TABLE;
	static const std::array<std::array<std::array<int, 8>, 8>, 12> END_GAME_POSITION_TABLE;
	static const int MATE_SCORE;
	static const int MIN;
	static const int MAX;
};