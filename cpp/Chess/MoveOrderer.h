#pragma once

#include <vector>
#include <set>
#include "Move.h"
#include "Board.h"
#include <functional>

enum MoveOrdererEnum
{
	HANDCRAFTED,
	TRAINING,
	MODEL,
};

class MoveOrderer
{
public:
	std::multiset<std::reference_wrapper<Move>> OrderMoves(Board& board, std::vector<Move>& moves, bool hashedMove, uint16_t bestMoveHash, std::array<uint16_t, 2> killerMoves,bool only_captures=false, bool useMVVLVA=true);
	virtual double MoveValue(Board* board, Move* move) = 0;
	~MoveOrderer()=default;
};

class MoveOrdererHandcrafted : public MoveOrderer {
public:
	virtual double MoveValue(Board* board, Move* move);
};

class MoveOrdererTraining : public MoveOrderer {
public:
	virtual double MoveValue(Board* board, Move* move);
};

