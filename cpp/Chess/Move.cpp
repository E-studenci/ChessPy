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
	this->score = 0;
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
	this->score = other.score;
	this->goodCapture = other.goodCapture;
}
std::string Move::ToString() const{
	std::string retString;
	if ((this->movingPiece == 1 || this->movingPiece == 7) &&
		(this->origin.column - this->destination.column == 2 || this->origin.column - this->destination.column == -2)) {
		if (this->origin.column - this->destination.column == 2)
			retString = "O-O-O";
		else
			retString = "O-O";
	}
	else {
		std::map<int, std::string> pieceMap{
			{0, " "},
			{7, "K"},
			{1, "K"},
			{8, "Q"},
			{2, "Q"},
			{9, "N"},
			{3, "N"},
			{10, "B"},
			{4, "B"},
			{11, "R"},
			{5, "R"},
			{12, "P"},
			{6, "P"} };
		if (this->movingPiece != 6 && this->movingPiece != 12)
			retString += pieceMap[this->movingPiece];
		retString += this->origin.ToString();
		retString += this->capturedPiece == 0 ? "-" : "x";
		retString += this->destination.ToString();
		if (this->promotion != 0)
			retString += "=" + pieceMap[this->promotion];
	}
	return retString;
}

Move* Move::Clone() {
	return new Move(*this);
}