

#include <stdio.h>
#include "def.h"

void PrintMoveList(const S_MOVELIST *list) {
	int index = 0;
	int score = 0;
	int move = 0;
	printf("MoveList:\n");

	for(index = 0; index < list->count; ++index) {

		move = list->moves[index].move;
		score = list->moves[index].score;

		printf("Move:%d > %s (score:%d)\n",index+1,PrMove(move),score);
	}
	printf("MoveList Total %d Moves:\n\n",list->count);
}




char *PrSq(const int sq) { // simple function, to describe squares from a bit array to something more "human" such as B2B3 etc...

	static char SqStr[3]; 

	int file = FilesBrd[sq]; 
	int rank = RanksBrd[sq];

	sprintf(SqStr, "%c%c", ('a'+file), ('1'+rank));

	return SqStr;

}

char *PrMove(const int move) {

	static char MvStr[6];

	int ff = FilesBrd[FROMSQ(move)];
	int rf = RanksBrd[FROMSQ(move)];
	int ft = FilesBrd[TOSQ(move)];
	int rt = RanksBrd[TOSQ(move)];

	int promoted = PROMOTED(move);

	if(promoted) { // if wze promote a pawn
		char pchar = 'q'; // it is a queen (most of the time)
		if(IsKn(promoted)) { // else we change it
			pchar = 'n';
		} else if(IsRQ(promoted) && !IsBQ(promoted))  {
			pchar = 'r';
		} else if(!IsRQ(promoted) && IsBQ(promoted))  {
			pchar = 'b';
		}
		sprintf(MvStr, "%c%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt), pchar);
	} else {
		sprintf(MvStr, "%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt));
	}

	return MvStr;
}


int ParseMove(char *ptrChar, S_BOARD *pos) {

	ASSERT(CheckBoard(pos));

	if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE; //we're assuming that the move thgat's being entered is at 
    if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;// least 4 characters long A2H2 means goes from A2 to H2
    if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;//and its the miniumm lenght of a move
    if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;



    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

	ASSERT(SqOnBoard(from) && SqOnBoard(to)); // make sure that squazre we found are actually okay

	S_MOVELIST list[1]; //create a movelist
    GenerateAllMoves(pos,list); //generate all the moves
    int MoveNum = 0;
	int Move = 0;
	int PromPce = EMPTY;

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) { // loop through ezvery move and find whioch onehas the same "from" and "to" squres
		//obivously they're unique except ifits a promotion
		Move = list->moves[MoveNum].move;
		if(FROMSQ(Move)==from && TOSQ(Move)==to) {
			PromPce = PROMOTED(Move); // which is managed here
			if(PromPce!=EMPTY) { // self explanatory
				if(IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4]=='r') {
					return Move;
				} else if(!IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4]=='b') {
					return Move;
				} else if(IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4]=='q') {
					return Move;
				} else if(IsKn(PromPce)&& ptrChar[4]=='n') {
					return Move;
				}
				continue;
			}
			return Move;
		}
    }

    return NOMOVE;
}
