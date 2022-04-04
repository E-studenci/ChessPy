#pragma once
class Move
{
public:
	int* from; // move from
	int* destination; // move destination
	int promotion; // 0 - none, 2-wq, 3-wn, 4-wb, 5-wr, 8-bq, 9-bn, 10-bb, 11-br
	int* capturedPiecePosition; // the position of the captured piece pre move
	int pieceMoved, pieceCaptured; // 1 - wk, 2 - wq, 3-wn, 4-wb, 5-wr, 6-wp, 7 - bk, 8 - bq, 9-bn, 10-bb, 11-br, 12-bp
	bool* castlingFlags; // castling flags pre move
	int* enPassant; // en passant flag pre move
	int* castleFrom; // if the move was a castling, it's the pre move position of the rook
	int* castleDestination; // if the move was a castling, it's the current position of the rook
	int seventyFiveMoveRule; // sfmr counter premove

	Move(int from[2], int destination[2], int promotion, int capturedPiecePosition[2], int pieceMoved, int pieceCaptured, bool castlingFlags[4], int castleFrom[2], int castleDestination[2], int seventyFiveMoveRule);
	Move(int from[2], int destination[2], int promotion=0) : Move(from, destination, promotion, nullptr, 0, 0, nullptr, nullptr, nullptr, 0) {}
};

