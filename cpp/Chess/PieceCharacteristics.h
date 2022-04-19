#pragma once
#include "Coordinates.h"
#include <vector>
struct PieceCharacteristics
{
	PieceCharacteristics() {}
	inline PieceCharacteristics(std::vector<Coordinates> pieceMovement, bool isSliding) {
		for (int i = 0; i < pieceMovement.size(); i++) {
			this->pieceMovement.push_back(Coordinates(pieceMovement[i]));
		}
		this->isSliding = isSliding;
	}
	std::vector<Coordinates> pieceMovement;
	bool isSliding;
};