#include "Move.h"
#include "Coordinates.h"
#include <array>
Move::Move(Coordinates from, Coordinates destination,
	int promotion, int movingPiece,
	std::array<bool,4> castlingFlags, Coordinates enPassant,
	int seventyFiveMoveRule) {
	this->from.row = from.row;
	this->from.column = from.column;
	this->destination.row = destination.row;
	this->destination.column = destination.column;
	this->movingPiece = movingPiece;
	this->castlingFlags = castlingFlags;
	this->enPassant.row = enPassant.row;
	this->enPassant.column = enPassant.column;
	this->promotion = promotion;
	this->seventyFiveMoveRule = seventyFiveMoveRule;
}
Move::Move(const Move& other) {
	this->from.row = other.from.row;
	this->from.column = other.from.column;
	this->destination.row = other.destination.row;
	this->destination.column = other.destination.column;
	this->movingPiece = other.movingPiece;
	this->castlingFlags = other.castlingFlags;
	this->enPassant.row = other.enPassant.row;
	this->enPassant.column = other.enPassant.column;
	this->promotion = other.promotion;
	this->seventyFiveMoveRule = other.seventyFiveMoveRule;
}
