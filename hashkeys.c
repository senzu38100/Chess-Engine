// hashkeys.c

#include "def.h"
#include "stdio.h"


U64 GeneratePosKey(const S_BOARD *pos){ //pointer to position or whatever position we wanna generate
    int sq = 0;
    U64 finalKey = 0; //the finalkey we're gonna be returning
    int piece = EMPTY;

    //pieces
    for(sq = 0; sq < BRD_SQ_NUM; ++sq){ //loop through all the squares
        piece = pos->pieces[sq]; //set piece equal to whatever value stored in the array at that square
        if(piece!=NO_SQ && piece!=EMPTY && piece != OFFBOARD){ //that value at the border will be NO_SQ so if its not a nosquare it must be in the chessboardand not empty must be a piece (black o white)
            ASSERT(piece>=wP && piece<=bK)
            finalKey ^= PieceKeys[piece][sq]; // we hash in whatever number stored
        }
    }

    if(pos->side == WHITE){ // we say if its white to move
        finalKey ^= SideKey;
    }

    if(pos->enPas != NO_SQ){
        ASSERT(pos->enPas>=0 && pos->enPas<BRD_SQ_NUM);
        finalKey ^= PieceKeys[EMPTY][pos->enPas];
    }

    ASSERT(pos->castlePerm>=0 && pos->castlePerm<=15); //chexk castleperm

    finalKey ^= CastleKeys[pos->castlePerm];

    return finalKey;

}