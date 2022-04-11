#pragma once
#include "Coordinates.h"
#include <array>
class Move
{
public:
	Coordinates from; // move from
	Coordinates destination; // move destination
	int promotion; // 0 - none, 2-wq, 3-wn, 4-wb, 5-wr, 8-bq, 9-bn, 10-bb, 11-br
	int movingPiece;
	std::array<bool,4> castlingFlags; // castling flags pre move
	Coordinates enPassant; // en passant flag pre move
	int seventyFiveMoveRule; // sfmr counter premove

	Move(Coordinates from, Coordinates destination, int promotion, int movingPiece, std::array<bool,4>castlingFlags, Coordinates enPassant, int seventyFiveMoveRule);
	Move(Coordinates from, Coordinates destination, int promotion = 0) : Move(from, destination, promotion, 0, std::array<bool, 4>{}, Coordinates{}, 0) {}
	Move(const Move& other);
};

