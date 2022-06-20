#include "TranspositionTable.h"

#include "Board.h"
void TranspositionTable::AddEntry(Board& board,EntryType type, int score, int depth, Move& bestMove)
{
	auto key = board.hash.Key();
	bool found = false;
	this->GetEntry(board, &found);
	if (found)
		if (this->table[key % table.size()].depth >= depth)
			return;
	this->table[key % table.size()].key = key;
	this->table[key % table.size()].move_hash = bestMove.Hash();
	this->table[key % table.size()].cutoff = type;
	this->table[key % table.size()].depth = depth;
	this->table[key % table.size()].score = score;
}

Entry TranspositionTable::GetEntry(const Board& board, bool* found)
{
	auto res = this->table[board.hash.Key() % table.size()];
	if (res.cutoff != EntryType::EMPTY_ENTRY) {
		if (res.key == board.hash.Key())
			this->hits++;
		else this->collisions++;
	}
	if (res.key == board.hash.Key()) {
		*found = true;
		return res;
	}
	return {};
}
