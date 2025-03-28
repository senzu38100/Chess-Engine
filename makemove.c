//makemove.c

#include "def.h"
#include <stdio.h>

#define HASH_PCE(pce,sq) (pos->posKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_CA (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= (SideKey))
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPas)]))


const int CastlePerm[120] = { // castle permsissions array
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
}; // its a mirror of our 120 squares board, all set to 15 except A1 E1 H1 8 E8 h8
// every time we move a piece, we take the catle persmission and we dop a bitwise AND operation with this array and the square from array



static void ClearPiece(const int sq, S_BOARD *pos) {

	ASSERT(SqOnBoard(sq)); // see if the square's on the board to acvoid problems
	ASSERT(CheckBoard(pos));
	
    int pce = pos->pieces[sq]; // and get the piece that's on that square
	
    ASSERT(PieceValid(pce)); // and double check if it's a piece (just in case)
	
	int col = PieceCol[pce];  //color of the piece
	int index = 0; // for looping
	int t_pceNum = -1; // will be xplained later down in the piece lsit
	
	ASSERT(SideValid(col));
	
    HASH_PCE(pce,sq); // XOR our pieces on that square
	
	pos->pieces[sq] = EMPTY; // set thatsquare to empty
    pos->material[col] -= PieceVal[pce]; //substract the value from the material score
	
	if(PieceBig[pce]) { //straight forward
			pos->bigPce[col]--;
		if(PieceMaj[pce]) {
			pos->majPce[col]--;
		} else {
			pos->minPce[col]--;
		}
	} else { // need to clear the bitboard for the pawns of the color of this piece
		CLRBIT(pos->pawns[col],SQ64(sq));
		CLRBIT(pos->pawns[BOTH],SQ64(sq));
	}
	
/*
for the next part: 
    pos->pceNum[wP] == 5 looping from 0 to 4
    pos->pList[pce][0] == sq0
    pos->pList[pce][1] == sq1
    pos->pList[pce][2] == sq2
    pos->pList[pce][3] == sq3
    pos->pList[pce][3] == sq4

so one of these square should be equal to the square that we put on argument
lets say it's sq3 so t_pceNum =3

*/

	for(index = 0; index < pos->pceNum[pce]; ++index) {
		if(pos->pList[pce][index] == sq) {
			t_pceNum = index;
			break;
		}
	}
	
	ASSERT(t_pceNum != -1); //another check which isn't necessary but jjust in case the checkboard function didnt do its job
	ASSERT(t_pceNum>=0&&t_pceNum<10);
	
	pos->pceNum[pce]--;		
    // so now, we have pos->pceNum[wP] == 4
	
	pos->pList[pce][t_pceNum] = pos->pList[pce][pos->pceNum[pce]];
    // pos->pList[pce][3] = pos->pList[wP][4] = sq4
    // so the square3 value has been deleted and that's how we remove a piece from the baord

}

static void AddPiece(const int sq, S_BOARD *pos, const int pce) { // basoically the opposite of clearpiece

    ASSERT(PieceValid(pce));
    ASSERT(SqOnBoard(sq));
	
	int col = PieceCol[pce];
	ASSERT(SideValid(col));

    HASH_PCE(pce,sq);
	
	pos->pieces[sq] = pce;

    if(PieceBig[pce]) {
			pos->bigPce[col]++;
		if(PieceMaj[pce]) {
			pos->majPce[col]++;
		} else {
			pos->minPce[col]++;
		}
	} else {
		SETBIT(pos->pawns[col],SQ64(sq));
		SETBIT(pos->pawns[BOTH],SQ64(sq));
	}
	
	pos->material[col] += PieceVal[pce];
	pos->pList[pce][pos->pceNum[pce]++] = sq;
	
}


static void MovePiece(const int from, const int to, S_BOARD *pos) {

    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
	
	int index = 0;
	int pce = pos->pieces[from];	
	int col = PieceCol[pce];
	ASSERT(SideValid(col));
    ASSERT(PieceValid(pce));
	
#ifdef DEBUG
	int t_PieceNum = FALSE; // make sure that we find what we're looking for
#endif

	HASH_PCE(pce,from);
	pos->pieces[from] = EMPTY;
	
	HASH_PCE(pce,to);
	pos->pieces[to] = pce;
	
	if(!PieceBig[pce]) {
		CLRBIT(pos->pawns[col],SQ64(from));
		CLRBIT(pos->pawns[BOTH],SQ64(from));
		SETBIT(pos->pawns[col],SQ64(to));
		SETBIT(pos->pawns[BOTH],SQ64(to));		
	}    
	
	for(index = 0; index < pos->pceNum[pce]; ++index) {
		if(pos->pList[pce][index] == from) {
			pos->pList[pce][index] = to;
#ifdef DEBUG
			t_PieceNum = TRUE;
#endif
			break;
		}
	}
	ASSERT(t_PieceNum);
}


int MakeMove(S_BOARD *pos, int move) {

	ASSERT(CheckBoard(pos));
	
	int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;
	int pce = pos->pieces[from];
	if (pce == EMPTY || PieceCol[pce] != side) {
		return FALSE;
	}
	
	ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(SideValid(side));
    ASSERT(PieceValid(pos->pieces[from]));
	// ASSERT(pos->hisPly >= 0 && pos->hisPly < MAXGAMEMOVES);
	// ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	pos->history[pos->hisPly].posKey = pos->posKey;
	
	if(move & MFLAGEP) { // if the move ends with the enpassant flag
        if(side == WHITE) {
            ClearPiece(to-10,pos); //remove the black pawn which is at the to square minus 10
        } else {
            ClearPiece(to+10,pos);//opposite if it's white's play
        }
    } else if (move & MFLAGCA) { // if its a castling move, check all possibilities
        switch(to) {
            case C1:
                MovePiece(A1, D1, pos);
			break;
            case C8:
                MovePiece(A8, D8, pos);
			break;
            case G1:
                MovePiece(H1, F1, pos);
			break;
            case G8:
                MovePiece(H8, F8, pos);
			break;
            default: ASSERT(FALSE); break;
        }
    }	
	
	if(pos->enPas != NO_SQ) HASH_EP; // if the current en passant square is set, then we'll hashout the en passant square
    HASH_CA; // we also hash the castling position
	
	pos->history[pos->hisPly].move = move;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPas = pos->enPas;
    pos->history[pos->hisPly].castlePerm = pos->castlePerm;

    pos->castlePerm &= CastlePerm[from];
    pos->castlePerm &= CastlePerm[to];
    pos->enPas = NO_SQ;

	HASH_CA;
	
	int captured = CAPTURED(move);
    pos->fiftyMove++;
	
	if(captured != EMPTY) {
        ASSERT(PieceValid(captured));
        ClearPiece(to, pos);
        pos->fiftyMove = 0;
    }
	
	pos->hisPly++; // self explanatory
	pos->ply++;
	
	// ASSERT(pos->hisPly >= 0 && pos->hisPly < MAXGAMEMOVES);
	// ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	if(PiecePawn[pos->pieces[from]]) { // set the new en passant square if its a pawn move
        pos->fiftyMove = 0;
        if(move & MFLAGPS) {
            if(side==WHITE) {
                pos->enPas=from+10;
                ASSERT(RanksBrd[pos->enPas]==RANK_3);
            } else {
                pos->enPas=from-10;
                ASSERT(RanksBrd[pos->enPas]==RANK_6);
            }
            HASH_EP;
        }
    }
	
	MovePiece(from, to, pos); // now we can actually move the piece
	
	int prPce = PROMOTED(move);
    if(prPce != EMPTY)   {
        ASSERT(PieceValid(prPce) && !PiecePawn[prPce]);
        ClearPiece(to, pos);
        AddPiece(to, pos, prPce);
    }
	
	if(PieceKing[pos->pieces[to]]) {
        pos->KingSq[pos->side] = to;
    }
	
	pos->side ^= 1;
    HASH_SIDE;

    ASSERT(CheckBoard(pos));
	
		
	if(SqAttacked(pos->KingSq[side],pos->side,pos))  {
        TakeMove(pos);
        return FALSE;
    }
	
	return TRUE;
	
}


void TakeMove(S_BOARD *pos) { // similar to makemove, not a lot to explain
	
	ASSERT(CheckBoard(pos));
	
	pos->hisPly--;
    pos->ply--;
	
	// ASSERT(pos->hisPly >= 0 && pos->hisPly < MAXGAMEMOVES);
	// ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
    int move = pos->history[pos->hisPly].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);	
	
	ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
	
	if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->castlePerm = pos->history[pos->hisPly].castlePerm;
    pos->fiftyMove = pos->history[pos->hisPly].fiftyMove;
    pos->enPas = pos->history[pos->hisPly].enPas;

    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->side ^= 1;
    HASH_SIDE;
	
	if(MFLAGEP & move) {
        if(pos->side == WHITE) {
            AddPiece(to-10, pos, bP);
        } else {
            AddPiece(to+10, pos, wP);
        }
    } else if(MFLAGCA & move) {
        switch(to) {
            case C1: MovePiece(D1, A1, pos); break;
            case C8: MovePiece(D8, A8, pos); break;
            case G1: MovePiece(F1, H1, pos); break;
            case G8: MovePiece(F8, H8, pos); break;
            default: ASSERT(FALSE); break;
        }
    }
	
	MovePiece(to, from, pos); //before capture
	
	if(PieceKing[pos->pieces[from]]) {
        pos->KingSq[pos->side] = from;
    }
	
	int captured = CAPTURED(move); // check if we captured anything
    if(captured != EMPTY) { 
        ASSERT(PieceValid(captured));
        AddPiece(to, pos, captured);
    }
	
	if(PROMOTED(move) != EMPTY)   {
        ASSERT(PieceValid(PROMOTED(move)) && !PiecePawn[PROMOTED(move)]);
        ClearPiece(from, pos); // clear the from position if we promoted
        AddPiece(from, pos, (PieceCol[PROMOTED(move)] == WHITE ? wP : bP));
    }
	
    ASSERT(CheckBoard(pos));

}
