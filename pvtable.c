#include <stdio.h>
#include "def.h"

const int PvSize = 0x100000 * 2;

int GetPvLine(const int depth, S_BOARD *pos) {

	ASSERT(depth < MAXDEPTH && depth >= 1);

	int move = ProbePvTable(pos);
	int count = 0;
	
	while(move != NOMOVE && count < depth) { //loop through all the moves until its an illegal move
    //or until we reached the maxdepth
	
		ASSERT(count < MAXDEPTH);
	
		if( MoveExists(pos, move) ) {  // if it exists, we add it in the array
			MakeMove(pos, move);
			pos->PvArray[count++] = move;
		} else {
			break;
		}		
		move = ProbePvTable(pos);	
	}
	
	while(pos->ply > 0) {
		TakeMove(pos);
	}
	
	return count;
	
}


void InitPvTable(S_PVTABLE *table) {
	table->numEntries = PvSize / sizeof(S_PVENTRY);
	table->numEntries -= 2;
	// free(table->pTable);
	table->pTable = (S_PVENTRY *) malloc(table->numEntries * sizeof(S_PVENTRY));
	printf("avant clear");
	ClearPvTable(table);
	printf("PvTable init complete with %d entries \n", table->numEntries);
}

void ClearPvTable(S_PVTABLE *table) {
	S_PVENTRY *pvEntry;

	for(pvEntry = table->pTable; pvEntry < table->pTable + table->numEntries; pvEntry++) {
		pvEntry->posKey = 0ULL;
		pvEntry->move = NOMOVE;
	}
}

void StorePvMove(const S_BOARD *pos, const int move) {

	int index = pos->posKey % pos->PvTable->numEntries;
	ASSERT(index >= 0 && index <= pos->PvTable->numEntries - 1);

	pos->PvTable->pTable[index].move = move;
	pos->PvTable->pTable[index].posKey = pos->posKey;

}



int ProbePvTable(const S_BOARD *pos) {

	int index = pos->posKey % pos->PvTable->numEntries;
	ASSERT(index >= 0 && index <= pos->PvTable->numEntries - 1);

	if( pos->PvTable->pTable[index].posKey == pos->posKey) {
		return pos->PvTable->pTable[index].move;
	}

	return NOMOVE;

}
