#include "move.h"
#include "Coordinates.h"
Move::Move(Coordinates from, Coordinates destination,
	int promotion, int movingPiece,
	bool castlingFlags[4], Coordinates enPassant,
	int seventyFiveMoveRule) {
	this->from.row = from.row;
	this->from.column = from.column;
	this->destination.row = destination.row;
	this->destination.column = destination.column;
	this->movingPiece = movingPiece;
	if (castlingFlags != nullptr) 
		this->castlingFlags = new bool[] {castlingFlags[0], castlingFlags[1], castlingFlags[2], castlingFlags[3]};
	this->enPassant.row = enPassant.row;
	this->enPassant.column = enPassant.column;
	this->promotion = promotion;
	this->seventyFiveMoveRule = seventyFiveMoveRule;
}
Move::~Move() {
	if (this->castlingFlags != nullptr){
		delete[] this->castlingFlags;
	}
}