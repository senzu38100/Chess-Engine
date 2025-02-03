// init.c
#include "def.h"
#include <stdlib.h>

#define RAND_64 (   (U64)rand() | \
                    (U64)rand() << 15 | \
                    (U64)rand() << 30 | \
                    (U64)rand() << 45 | \
                    ((U64)rand() & 0xf) << 60    )
//filling 64 bits woth random numbers
//generates a 15 bit random number
// first line: 0000 000000000000000 000000000000000 000000000000000 11111111111111
// seconde line // 0000 000000000000000 000000000000000 11111111111111  000000000000000
//and so on... shifting it 15 to the left
//leaves with 4 bits at the end and we take another random number and end it with hexa
// so we fill the 64 BIT at the end

int Sq120ToSq64[BRD_SQ_NUM]; 
int Sq64ToSq120[64];

U64 SetMask[64]; //see at def.h
U64 ClearMask[64];

U64 PieceKeys[13][120]; //#pieces index and each sqaure
U64 SideKey; //hashing in a random number if its white to move
// 0 0 0 0
U64 CastleKeys[16]; // 16 castle cases cuz bits are worth 1+2+4+8 = 16

int FilesBrd[BRD_SQ_NUM];
int RanksBrd[BRD_SQ_NUM];

void InitFilesRanksBrd() {
    int index = 0;
    int file = FILE_A;
    int rank = RANK_8;
    int sq = A1;
    int sq64 = 0;

    for(index = 0; index < BRD_SQ_NUM; index ++) {
        FilesBrd[index] = OFFBOARD; //we set to offboard every position in the array
        RanksBrd[index] = OFFBOARD; //including the gray sqares
    }

    for(rank = RANK_1; rank <= RANK_8; ++rank) {
        for(file = FILE_A; file <= FILE_H; ++file) {
            sq = FR2SQ(file, rank);
            FilesBrd[sq] = file;
            RanksBrd[sq] = rank;
        }
    }
}

void InitHashKeys(){ //couple of loops filling our variables with random numbers
    int index = 0;
    int index2 = 0;
    for(index = 0; index < 13; index++){
        for(index2 = 0; index2 < 120; ++index2){
            PieceKeys[index][index2] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for(index = 0; index < 16; index++){
        CastleKeys[index] = RAND_64;
    }
}


void InitBitMasks(){
    int index = 0;
    for(index = 0; index <64; index ++){
        SetMask[index] = 0ULL;
        ClearMask[index] = 0ULL;
    }
    for(index = 0; index <64; index ++){
        SetMask[index] |= (1ULL << index); // shifted by index
        ClearMask[index] = ~SetMask[index]; //everything thats a 0 becomes a 1 and everything thats a 1 becomes a 0
    }
}
void InitSq120To64(){
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;

    for (index = 0; index < BRD_SQ_NUM; index++){
        Sq120ToSq64[index] = 65; //indexing them all to 65 becuz thge max is 63 (0 to 63 cases)
    }

    for (index = 0; index < 64; index ++){
        Sq64ToSq120[index] = 120; // same as 65 but for the 120 array
    }
    
    for(rank = RANK_1; rank <= RANK_8; rank++){ // we browse through all the ranks
        for(file=FILE_A; file <= FILE_H; file++){ // .... the files
            sq = FR2SQ(file, rank); // fpor each coordinate (file, rank), we assign it a square in the 64 array
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++; // at the end sq64 will represent the 64 base index array for that specific square
        }
    }
}

void AllInit(){
    InitSq120To64();
    InitBitMasks();
    InitHashKeys();
    InitFilesRanksBrd();
}