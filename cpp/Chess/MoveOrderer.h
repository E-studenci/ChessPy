#pragma once

#include <vector>
#include <set>
#include "Move.h"
#include "Board.h"
#include <functional>
class MoveOrderer
{
public:
	std::multiset<std::reference_wrapper<Move>> OrderMoves(const Board& board, std::map<Coordinates, std::vector<Move>>& moves, bool hashedMove, uint16_t bestMoveHash, bool only_captures=false);
	virtual double MoveValue(const Board& board, Move& move) = 0;
	~MoveOrderer()=default;
};

class MoveOrdererHandcrafted : public MoveOrderer {
public:
	virtual double MoveValue(const Board& board, Move& move);
};