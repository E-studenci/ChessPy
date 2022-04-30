#pragma once
#include "Coordinates.h"
#include <array>
#include <string>
class Move
{
public:
	Coordinates origin;		 // move origin
	Coordinates destination; // move destination
	int promotion;			 // 0 - none, 2-wq, 3-wn, 4-wb, 5-wr, 8-bq, 9-bn, 10-bb, 11-br
	int movingPiece;
	int capturedPiece;
	std::array<bool, 4> castlingFlags; // castling flags pre move
	Coordinates enPassant;			   // en passant flag pre move
	int seventyFiveMoveRule;		   // sfmr counter premove

	Move(Coordinates origin, Coordinates destination, int promotion, int movingPiece, int capturedPiece, std::array<bool, 4> castlingFlags, Coordinates enPassant, int seventyFiveMoveRule);
	Move(Coordinates origin, Coordinates destination, int promotion = 0, int movingPiece = 0, int capturedPiece = 0) : Move(origin, destination, promotion, movingPiece,capturedPiece, std::array<bool, 4>{}, Coordinates{}, 0){};
	Move(const Move &other);
	std::string ToString() const;
};
