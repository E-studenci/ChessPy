#pragma once
#include "Coordinates.h"


class Zobrist
{
public:
    Zobrist() {
        this->key = 0;
    }
    Zobrist(const Zobrist& other) {
        this->key = other.key;
    }
    void operator=(const Zobrist& other) {
        this->key = other.key;
    }
    void Recalculate(class Board* board) { key = Generate(*board); }
    uint64_t Key() const { return key; }

    void ToggleSTM() { key ^= ZobristTable[12 * 64]; }
    void ToggleCastle(int castle) { key ^= ZobristTable[12 * 64 + castle + 1]; }
    void ToggleEnpassant(int column) { key ^= ZobristTable[12 * 64 + 5 + column]; }
    void TogglePieceSquare(int piece, Coordinates coordinates) { key ^= ZobristTable[(piece-1) * 64 + coordinates.row * 8 + coordinates.column]; }

    bool Verify(const class Board& board) const { return Generate(board) == key; }
private:
    static uint64_t Generate(const class Board& board);

    // 12 pieces * 64 squares, 1 for side to move, 4 for casteling rights and 8 for ep squares
    const static std::array<uint64_t, 781> ZobristTable;

    uint64_t key = 0;
};

