#pragma once
#include "PieceCharacteristics.h"
#include <map>
#include <tuple>
#include <vector>

class PieceMovement
{
public:
	static PieceCharacteristics Get(int piece);
private:
	std::map<int, PieceCharacteristics> pieceMovementMap;
	inline static PieceMovement* singleton = nullptr;
	// for every piece (1-11): 
	//		array containing directions (for example {[-1,-2], [-2,-1],..} for knights), 
	//		isSliding flag
	PieceMovement();
};

