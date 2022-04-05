#include "move.h"
Move::Move(int from[2], int destination[2],
	int promotion, bool castlingFlags[4],
	int enPassant[2],int seventyFiveMoveRule) {
	this->from = from;
	this->destination = destination;
	this->promotion = promotion;
	this->castlingFlags = castlingFlags;
	this->enPassant = enPassant;
	this->seventyFiveMoveRule = seventyFiveMoveRule;
}
