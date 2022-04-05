#pragma once
class Move
{
public:
	int* from; // move from
	int* destination; // move destination
	int promotion; // 0 - none, 2-wq, 3-wn, 4-wb, 5-wr, 8-bq, 9-bn, 10-bb, 11-br
	bool* castlingFlags; // castling flags pre move
	int* enPassant; // en passant flag pre move
	int seventyFiveMoveRule; // sfmr counter premove

	Move(int from[2], int destination[2], int promotion, bool castlingFlags[4], int enPassant[2], int seventyFiveMoveRule);
	Move(int from[2], int destination[2], int promotion=0) : Move(from, destination, promotion, nullptr, nullptr, 0) {}
};

