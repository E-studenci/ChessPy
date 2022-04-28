#include "Move.h"
#include "Coordinates.h"
#include <array>
#include <string>
Move::Move(Coordinates from, Coordinates destination,
	int promotion, int movingPiece, int capturedPiece,
	std::array<bool,4> castlingFlags, Coordinates enPassant,
	int seventyFiveMoveRule) {
	this->origin.row = from.row;
	this->origin.column = from.column;
	this->destination.row = destination.row;
	this->destination.column = destination.column;
	this->movingPiece = movingPiece;
	this->capturedPiece = capturedPiece;
	this->castlingFlags = castlingFlags;
	this->enPassant.row = enPassant.row;
	this->enPassant.column = enPassant.column;
	this->promotion = promotion;
	this->seventyFiveMoveRule = seventyFiveMoveRule;
}
Move::Move(const Move& other) {
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
}
std::string Move::ToString() const{
	std::string retString;
	retString += "\n";
	retString += this->origin.ToString();
	retString += "->";
	retString += this->destination.ToString();
	retString += "moving piece: ";
	retString += this->movingPiece;
	retString += "captured piece: ";
	retString += this->capturedPiece;
	return retString;
}