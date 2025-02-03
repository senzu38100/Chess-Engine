#include <stdio.h>
#include <stdlib.h>
#include "def.h"

#define WAC1 "6k1/5pp1/p7/3p2Pp/P2qbp1P/6r1/1R2Q2K/4R3 w - - "
#define WAC2 "r4r2/pbq1npk1/1p2pNp1/4P3/3p2P1/5N2/PPPQ1PK1/R4R2 w - -"
#define WAC3 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define WAC4  "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1"
int main(){
    AllInit();
    S_BOARD board[1];
    S_MOVELIST list[1];
    S_SEARCHINFO info[1];
    ParseFen(WAC4, board);
    printf("test3");
    char input[6];
    int Move = NOMOVE;
    int PvNum = 0;
    int Max = 0;
    while(TRUE) {
        PrintBoard(board);
        printf("Rentre ton move >");
        fgets(input, 6, stdin);

        if(input[0]=='q') {
            break;
        } else if(input[0]=='t') {
            TakeMove(board);
        } else if(input[0] == 's') {
            info->depth = 6;
            SearchPosition(board, info);
        } else {
            Move = ParseMove(input, board);
            if(Move != NOMOVE) {
                StorePvMove(board, Move);
                MakeMove(board, Move);
            } else {
                printf("Move Not Parsed:%s\n", input);
            }
        }

        fflush(stdin);
    }

    
    return 0;
} 

