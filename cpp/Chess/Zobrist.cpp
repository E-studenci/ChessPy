#include "Zobrist.h"
#include <cstddef>
#include <random>
#include "Board.h"
#include "Coordinates.h"

const std::array<uint64_t, 781> Zobrist::ZobristTable = [] {
    std::array<uint64_t, 781> table;

    std::mt19937_64 gen(0);
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    for (size_t i = 0; i < table.size(); i++)
    {
        table[i] = dist(gen);
    }

    return table;
}();

uint64_t Zobrist::Generate(const Board& board)
{
    uint64_t Key = 0;
    for (int row = 0;row < 8;row++) {
        for (int column = 0; column < 8; column++) {
            if (board.board[row][column] != 0) {
                uint64_t piece = board.board[row][column] -1;
                Key ^= ZobristTable[piece * 64 + row * 8 + column];
            }
        }
    }

    if (board.sideToMove == 0)
        Key ^= ZobristTable.at(12 * 64);

    if (board.castlingFlags[0])
        Key ^= ZobristTable.at(12 * 64 + 1);
    if (board.castlingFlags[1])
        Key ^= ZobristTable.at(12 * 64 + 2);
    if (board.castlingFlags[2])
        Key ^= ZobristTable.at(12 * 64 + 3);
    if (board.castlingFlags[3])
        Key ^= ZobristTable.at(12 * 64 + 4);
    if (board.enPassant)
    {
        Key ^= ZobristTable.at(12 * 64 + 5 + board.enPassant.column);
    }

    return Key;
}