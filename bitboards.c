#include <stdio.h>
#include "def.h"


const int BitTable[64] = {
    63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2, 
    51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
     26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
      58, 20, 37, 17, 36, 8
}; //

int PopBit(U64 *bb) { // takestakes the first bit in the least significant bit in the bitboard
    U64 b = *bb ^ (*bb -1); // and return the index of tihs bit and set it to 0
    unsigned int fold = (unsigned) ((b & 0xffffffff) ^(b >> 32));
    *bb &= (*bb -1);
    return BitTable[(fold * 0x783a9b23) >> 26];
}

int CountBits(U64 b) { //counts and return the number of 1s in the bitboard
    int r;
    for(r = 0; b; r++, b &= b-1);
    return r;
}

void PrintBitBoard(U64 bb){
    U64 shiftMe = 1ULL; //long long cuz we need more than 32 bits

    int rank =0;
    int file = 0;
    int sq = 0;
    int sq64 = 0;

    printf("\n");
    for(rank = RANK_8; rank >= RANK_1; --rank){ //starting at the 8th rank and file A
        for (file = FILE_A; file <= FILE_H; ++file){ //so on to 1st rank and file H
            sq = FR2SQ(file, rank); // the 120 base index
            sq64 = SQ64(sq); // need the 64 base index cuz we gon use our 64 bit number
            if((shiftMe << sq64) & bb) //so if there's a bit on there, print ax X
                printf("X");
            else   
                printf("-"); //otherwise print a minus
        }
        printf("\n");
    }
    printf("\n");
}