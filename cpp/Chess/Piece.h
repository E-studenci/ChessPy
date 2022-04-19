#pragma once
#include <vector>
class Piece
{
public:
	std::vector<int[2]> directions;
	bool isSliding;
	bool canEnPassant;
	bool canCastle;
};

