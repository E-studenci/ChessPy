#pragma once
#include <cstdint>
#include <vector>

#include "Move.h"
#include "Board.h"
enum class EntryType : char {
	EMPTY_ENTRY,
	EXACT,
	LOWERBOUND,
	UPPERBOUND
};


class Entry {
public:

	Entry(uint64_t key,
	int16_t score,
	uint8_t depth,
	uint16_t move_hash,
	EntryType cutoff) {
		this->key = key;
		this->score = score;
		this->depth = depth;
		this->move_hash = move_hash;
		this->cutoff = cutoff;
	}
	Entry() {
	}
	Entry(const Entry &other) {
		this->key = other.key;
		this->score = other.score;
		this->depth = other.depth;
		this->move_hash = other.move_hash;
		this->cutoff = other.cutoff;
	}
	void operator=(const Entry& other) {
		this->key = other.key;
		this->score = other.score;
		this->depth = other.depth;
		this->move_hash = other.move_hash;
		this->cutoff = other.cutoff;
	}
	uint64_t key;

	int16_t score;
	uint8_t depth;
	uint16_t move_hash;
	EntryType cutoff;
};
class TranspositionTable
{
public:
	void init(int MB = 10) {
		this->table.clear();
		this->table.resize(static_cast<unsigned long long>(MB) * 1024 * 1024 / sizeof(Entry));
	}
	void AddEntry(Board& board, EntryType type, int score, int depth, Move& bestMove);
	Entry GetEntry(const Board& board, bool* found);
private:
	std::vector<Entry> table;
};

