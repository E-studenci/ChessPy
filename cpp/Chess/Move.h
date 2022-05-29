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
	double score;

	friend bool operator<(Move const& lhs, Move const& rhs) {
		return lhs.score > rhs.score;
	}
	uint16_t Hash() const {
		uint16_t res = 0;
		if (this->promotion != 0) {
			uint8_t prom = (this->promotion == 2 || this->promotion == 8) ? 1 :
				(this->promotion == 3 || this->promotion == 9) ? 2 :
				(this->promotion == 4 || this->promotion == 10) ? 3 : 4;
			res |= (prom << uint8_t{ 12 });
		}
		res|= (this->origin.row << uint8_t{ 9 });
		res |= (this->origin.column << uint8_t{ 6 });
		res |= (this->destination.row << uint8_t{ 3 });
		res |= (this->destination.column);
		return res;
	}
	void operator=(const Move& other) {
		this->origin.row = other.origin.row;
		this->origin.column = other.origin.column;
		this->destination.row = other.destination.row;
		this->destination.column = other.destination.column;
		this->movingPiece = other.movingPiece;
		this->capturedPiece = other.capturedPiece;
		this->castlingFlags = other.castlingFlags;
		this->enPassant.row = other.enPassant.row;
		this->enPassant.column = other.enPassant.column;
		this->promotion = other.promotion;
		this->seventyFiveMoveRule = other.seventyFiveMoveRule;
		this->score = other.score;
	}
	Move(Coordinates origin, Coordinates destination, int promotion, int movingPiece, int capturedPiece, std::array<bool, 4> castlingFlags, Coordinates enPassant, int seventyFiveMoveRule);
	Move(Coordinates origin, Coordinates destination, int promotion = 0, int movingPiece = 0, int capturedPiece = 0) : Move(origin, destination, promotion, movingPiece,capturedPiece, std::array<bool, 4>{}, Coordinates{}, 0){};
	Move(const Move &other);
	Move() : Move(Coordinates{-1,-1}, Coordinates{ -1,-1 }, 0, movingPiece, capturedPiece, std::array<bool, 4>{}, Coordinates{}, 0) {};

	Move* Clone();
	std::string ToString() const;
};
