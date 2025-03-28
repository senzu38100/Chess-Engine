#include <stdio.h>
#include <stdlib.h>
#include "def.h"
#include <string.h>


#define WAC1 "6k1/5pp1/p7/3p2Pp/P2qbp1P/6r1/1R2Q2K/4R3 w - - "
#define WAC2 "r4r2/pbq1npk1/1p2pNp1/4P3/3p2P1/5N2/PPPQ1PK1/R4R2 w - -"
#define WAC3 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define WAC4  "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1"
int main(){
	AllInit();
	
	S_BOARD pos[1];
	S_SEARCHINFO info[1];
	InitPvTable(pos->PvTable);


	printf("Welcome to senzuUI! type 'Vice' for console mode...\n");

	char line[256];
	while(TRUE) {
		memset(&line[0], 0, sizeof(line));

		fflush(stdout);
		if(!fgets(line, 256, stdin))
			continue;
		if (line[0] == '\n')
			continue;
		if (!strncmp(line, "uci", 3)) {
			Uci_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "xboard", 6)) {
			XBoard_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if(!strncmp(line, "vice", 4)) {
			Console_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if(!strncmp(line, "quit", 4)) {
			break;
		}
	}


	free(pos->PvTable->pTable);
	return 0;
} 


