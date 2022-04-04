#include "move.h"
Move::Move(int from[2], int destination[2], 
	int promotion, int capturedPiecePosition[2], 
	int pieceMoved, int pieceCaptured, 
	bool castlingFlags[4], int castleFrom[2], 
	int castleDestination[2], int seventyFiveMoveRule) {
	this->from = from;
	this->destination = destination;
	this->promotion = promotion;
	this->capturedPiecePosition = capturedPiecePosition;
	this->pieceMoved = pieceMoved;
	this->pieceCaptured = pieceCaptured;
	this->castlingFlags = castlingFlags;
	this->castleFrom = castleFrom;
	this->castleDestination = castleDestination;
	this->seventyFiveMoveRule = seventyFiveMoveRule;
}
