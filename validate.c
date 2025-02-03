
#include "def.h"



int SqOnBoard(const int sq) { //checks if the square (which is supposed to be part of the 64 square array) is actually one of those
	return FilesBrd[sq]==OFFBOARD ? 0 : 1;
}

int SideValid(const int side) { // same for the side
	return (side==WHITE || side == BLACK) ? 1 : 0;
}

int FileRankValid(const int fr) { // for file and rank
	return (fr >= 0 && fr <= 7) ? 1 : 0;
}

int PieceValidEmpty(const int pce) { // basic, but it just says that any piece is more valuable than an empty square and less than the king (useless for a human but necessary for a computer)
	return (pce >= EMPTY && pce <= bK) ? 1 : 0;
}

int PieceValid(const int pce) { // same but wiuthout the empty
	return (pce >= wP && pce <= bK) ? 1 : 0;
}
