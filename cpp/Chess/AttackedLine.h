#pragma once
#include <vector>
#include "Coordinates.h"
struct AttackedLine
{
	AttackedLine() {}
	inline AttackedLine(std::vector<Coordinates> attackedLine, Coordinates pinnedPiece = Coordinates(-1, -1)) {
		this->attackedLine = attackedLine;
		this->pinnedPiece = pinnedPiece;
	}
	std::vector<Coordinates> attackedLine;
	Coordinates pinnedPiece;
};