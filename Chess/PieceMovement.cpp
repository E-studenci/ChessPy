#include "PieceMovement.h"
#include "Coordinates.h"
#include <vector>
PieceCharacteristics PieceMovement::Get(int piece) {
	if (singleton == nullptr) {
		singleton = new PieceMovement();
	}
	return singleton->pieceMovementMap[piece];
}
PieceMovement::PieceMovement() {
	// directions
	Coordinates u{ -1,0 };
	Coordinates ur{ -1,1 };
	Coordinates r{ 0,1 };
	Coordinates dr{ 1,1 };
	Coordinates d{ 1,0 };
	Coordinates dl{ 1,-1 };
	Coordinates l{ 0,-1 };
	Coordinates ul{ -1,-1 };

	Coordinates uur{ -2, 1 };
	Coordinates rru{ -1, 2 };
	Coordinates rrd{ 1, 2 };
	Coordinates ddr{ 2, 1 };
	Coordinates ddl{ 2, -1 };
	Coordinates lld{ 1, -2 };
	Coordinates llu{ -1, -2 };
	Coordinates uul{ -2, -1 };
	// /directions

	std::vector<Coordinates> wkMovement{ u,ur,r,dr,d,dl,l,ul };
	std::vector<Coordinates> wqMovement{ u,ur,r,dr,d,dl,l,ul };
	std::vector<Coordinates> wnMovement{ uur, rru,rrd,ddr,ddl,lld,llu,uul };
	std::vector<Coordinates> wbMovement{ ur,dr,dl,ul };
	std::vector<Coordinates> wrMovement{ u,r,d,l };
	std::vector<Coordinates> wpMovement{ u };
	std::vector<Coordinates> bkMovement{ u,ur,r,dr,d,dl,l,ul };
	std::vector<Coordinates> bqMovement{ u,ur,r,dr,d,dl,l,ul };
	std::vector<Coordinates> bnMovement{ uur, rru,rrd,ddr,ddl,lld,llu,uul };
	std::vector<Coordinates> bbMovement{ ur,dr,dl,ul };
	std::vector<Coordinates> brMovement{ u,r,d,l };
	std::vector<Coordinates> bpMovement{ d };

	PieceCharacteristics wk(wkMovement, false);
	PieceCharacteristics wq(wqMovement, true);
	PieceCharacteristics wn(wnMovement, false);
	PieceCharacteristics wb(wbMovement, true);
	PieceCharacteristics wr(wrMovement, true);
	PieceCharacteristics wp(wpMovement, false);
	PieceCharacteristics bk(bkMovement, false); 
	PieceCharacteristics bq(bqMovement, true);
	PieceCharacteristics bn(bnMovement, false);
	PieceCharacteristics bb(bbMovement, true);
	PieceCharacteristics br(brMovement, true);
	PieceCharacteristics bp(bpMovement, false);

	this->pieceMovementMap.insert({ 1, wk });
	this->pieceMovementMap.insert({ 2, wq });
	this->pieceMovementMap.insert({ 3, wn });
	this->pieceMovementMap.insert({ 4, wb });
	this->pieceMovementMap.insert({ 5, wr });
	this->pieceMovementMap.insert({ 6, wp });
	this->pieceMovementMap.insert({ 7, bk });
	this->pieceMovementMap.insert({ 8, bq });
	this->pieceMovementMap.insert({ 9, bn });
	this->pieceMovementMap.insert({ 10, bb });
	this->pieceMovementMap.insert({ 11, br });
	this->pieceMovementMap.insert({ 12, bp });
}