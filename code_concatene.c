/* ========================================== */
/* FICHIER : attack.c */
/* ========================================== */

// attack.c

#include <stdio.h>
#include "def.h"

const int KnDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 }; //looking at the board representation in the excel file, knights are positionned like this from the current square
const int RkDir[4] = { -1, -10,	1, 10 }; // for rooks,, its simple, its just +9
const int BiDir[4] = { -9, -11, 11, 9 }; // for bishops
const int KiDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 }; //king direction can move only one square to every direction

int SqAttacked(const int sq, const int side, const S_BOARD *pos) {
    //what square, which side is attacking it and in what position basically

	int pce,index,t_sq,dir;
	
	ASSERT(SqOnBoard(sq));
	ASSERT(SideValid(side));
	ASSERT(CheckBoard(pos));
	
	// pawns
	if(side == WHITE) {
		if(pos->pieces[sq-11] == wP || pos->pieces[sq-9] == wP) {
			return TRUE;
		}
	} else {
		if(pos->pieces[sq+11] == bP || pos->pieces[sq+9] == bP) {
			return TRUE;
		}	
	}
	
	// knights
	for(index = 0; index < 8; ++index) {		
		pce = pos->pieces[sq + KnDir[index]];
		// ASSERT(PceValidEmptyOffbrd(pce));
		if(pce != OFFBOARD && IsKn(pce) && PieceCol[pce]==side) {
			return TRUE;
		}
	}
	
	// rooks, queens
	for(index = 0; index < 4; ++index) {		
		dir = RkDir[index];
		t_sq = sq + dir;
		// ASSERT(SqIs120(t_sq));
		pce = pos->pieces[t_sq];
		// ASSERT(PceValidEmptyOffbrd(pce));
		while(pce != OFFBOARD) {
			if(pce != EMPTY) {
				if(IsRQ(pce) && PieceCol[pce] == side) {
					return TRUE;
				}
				break;
			}
			t_sq += dir;
			// ASSERT(SqIs120(t_sq));
			pce = pos->pieces[t_sq];
		}
	}
	
	// bishops, queens
	for(index = 0; index < 4; ++index) {		
		dir = BiDir[index];
		t_sq = sq + dir;
		// ASSERT(SqIs120(t_sq));
		pce = pos->pieces[t_sq];
		// ASSERT(PceValidEmptyOffbrd(pce));
		while(pce != OFFBOARD) {
			if(pce != EMPTY) {
				if(IsBQ(pce) && PieceCol[pce] == side) {
					return TRUE;
				}
				break;
			}
			t_sq += dir;
			// ASSERT(SqIs120(t_sq));
			pce = pos->pieces[t_sq];
		}
	}
	
	// kings
	for(index = 0; index < 8; ++index) {		
		pce = pos->pieces[sq + KiDir[index]];
		// ASSERT(PceValidEmptyOffbrd(pce));
		if(pce != OFFBOARD && IsKi(pce) && PieceCol[pce]==side) {
			return TRUE;
		}
	}
	
	return FALSE;
	
}


/* ========================================== */
/* FICHIER : Board.c */
/* ========================================== */

//board.c

#include <stdio.h>
#include "def.h"


int CheckBoard(const S_BOARD *pos) { //were gonna mirror the position we're in

	int t_pceNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // t_ means temporary
	int t_bigPce[2] = { 0, 0}; //self explanbatory
	int t_majPce[2] = { 0, 0};
	int t_minPce[2] = { 0, 0};
	int t_material[2] = { 0, 0};

	int sq64,t_piece,t_pce_num,sq120,colour,pcount;

	U64 t_pawns[3] = {0ULL, 0ULL, 0ULL};

	t_pawns[WHITE] = pos->pawns[WHITE];
	t_pawns[BLACK] = pos->pawns[BLACK];
	t_pawns[BOTH] = pos->pawns[BOTH];

    // check piece lists
	for(t_piece = wP; t_piece <= bK; ++t_piece) { //looops by piece type from white pawn to black king
		for(t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; ++t_pce_num) { //loops by the poiuece number for each type
			sq120 = pos->pList[t_piece][t_pce_num]; //we get the squarz for that piece
			ASSERT(pos->pieces[sq120]==t_piece); // we assert that if our piece list says a white pawn on this square then our pieces array should have a white pawn on it (they should be the same)
		}                                           //if not, we kjnow that the piece list isnt aligned with what's on the board array
	}

    // check piece count and other counters
	for(sq64 = 0; sq64 < 64; ++sq64) {
		sq120 = SQ120(sq64);
		t_piece = pos->pieces[sq120]; //looking on the piece on that square
		t_pceNum[t_piece]++; //incrementing the number of temporary pieces for that piece type
		colour = PieceCol[t_piece]; //getting the colour of that piece
		if( PieceBig[t_piece] == TRUE) t_bigPce[colour]++; //incrementing depending which typoe of piece it is
		if( PieceMin[t_piece] == TRUE) t_minPce[colour]++;
		if( PieceMaj[t_piece] == TRUE) t_majPce[colour]++;

		t_material[colour] += PieceVal[t_piece]; //then we upodate the material value 
	}

    for(t_piece = wP; t_piece <= bK; ++t_piece) {
    //     printf("Checking piece: %d, t_pceNum: %d, pos->pceNum: %d\n",
    //    t_piece, t_pceNum[t_piece], pos->pceNum[t_piece]);
		ASSERT(t_pceNum[t_piece]==pos->pceNum[t_piece]); //the number of pieces we found on the board and stored inside our temporary piece number array equals for each piece type what our positiojn says
	}                                                   // if they're not equal, something went wrong with the piece number array in our position structure 
    
    // check bitboards count
	pcount = CNT(t_pawns[WHITE]); // we take the paw count for the white bitboard
	ASSERT(pcount == pos->pceNum[wP]); // and we assert that count is equal to what the position says is the number of white pawns we have
	pcount = CNT(t_pawns[BLACK]); //same for black
	ASSERT(pcount == pos->pceNum[bP]);
	pcount = CNT(t_pawns[BOTH]); //same for both
	ASSERT(pcount == (pos->pceNum[bP] + pos->pceNum[wP]));


    // check bitboards squares
	while(t_pawns[WHITE]) { //checking if theres a white, black or both pawn oin that square otherwise thre bits don't match with the pieces array in our position and we throw an assert error
		sq64 = POP(&t_pawns[WHITE]);
		ASSERT(pos->pieces[SQ120(sq64)] == wP); //loop keeps going until there is not bits left
	}

	while(t_pawns[BLACK]) {
		sq64 = POP(&t_pawns[BLACK]); //same
		ASSERT(pos->pieces[SQ120(sq64)] == bP);
	}

	while(t_pawns[BOTH]) { //same
		sq64 = POP(&t_pawns[BOTH]);
		ASSERT( (pos->pieces[SQ120(sq64)] == bP) || (pos->pieces[SQ120(sq64)] == wP) );
	}

    ASSERT(t_material[WHITE]==pos->material[WHITE] && t_material[BLACK]==pos->material[BLACK]);
	ASSERT(t_minPce[WHITE]==pos->minPce[WHITE] && t_minPce[BLACK]==pos->minPce[BLACK]);
	ASSERT(t_majPce[WHITE]==pos->majPce[WHITE] && t_majPce[BLACK]==pos->majPce[BLACK]);
	ASSERT(t_bigPce[WHITE]==pos->bigPce[WHITE] && t_bigPce[BLACK]==pos->bigPce[BLACK]);

	ASSERT(pos->side==WHITE || pos->side==BLACK);
	ASSERT(GeneratePosKey(pos)==pos->posKey); //the psition key should be the same as the genreated key
                                                //when we make a move, we don't at the end of making a move regenerate the entire hashkey becuz thats too many computations
                                                //so we simply XOR a piece that weve moved into the new square


    ASSERT(pos->enPas==NO_SQ || ( RanksBrd[pos->enPas]==RANK_6 && pos->side == WHITE) //check for en passant (its either no square or it must be a square that if the side to move is white
                                                                                      // is on the 6th rank if the side to move is black then the en passant square must be on the 3rd rank
		 || ( RanksBrd[pos->enPas]==RANK_3 && pos->side == BLACK));

	ASSERT(pos->pieces[pos->KingSq[WHITE]] == wK); //checking the square of kings if they are the corresponding ones  
	ASSERT(pos->pieces[pos->KingSq[BLACK]] == bK);

	ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

	// ASSERT(PceListOk(pos));

	return TRUE; //assuming we passed everything, we return true 
}

void UpdateListsMaterial(S_BOARD *pos) {

	int piece,sq,index,colour;

	for(index = 0; index < BRD_SQ_NUM; ++index) {
		sq = index;
		piece = pos->pieces[index];
		if(piece!=OFFBOARD && piece!= EMPTY) { //if not offboard and empty
			colour = PieceCol[piece];

		    if( PieceBig[piece] == TRUE) pos->bigPce[colour]++; 
		    if( PieceMin[piece] == TRUE) pos->minPce[colour]++;
		    if( PieceMaj[piece] == TRUE) pos->majPce[colour]++;

			pos->material[colour] += PieceVal[piece];

            //for example : pList[wP][0] = a1;
            // pList[wP][1] = a2;

			ASSERT(pos->pceNum[piece] < 10 && pos->pceNum[piece] >= 0);

			pos->pList[piece][pos->pceNum[piece]] = sq;
			pos->pceNum[piece]++;


			if(piece==wK) pos->KingSq[WHITE] = sq;
			if(piece==bK) pos->KingSq[BLACK] = sq;

			if(piece==wP) { //if the piece is a white pawn, set the bit on the bitboard in a position for white pawn and both
				SETBIT(pos->pawns[WHITE],SQ64(sq)); //we coinvert it cuz sq is in a 120 format 
				SETBIT(pos->pawns[BOTH],SQ64(sq));
			} else if(piece==bP) { //same for black pawn
				SETBIT(pos->pawns[BLACK],SQ64(sq)); 
				SETBIT(pos->pawns[BOTH],SQ64(sq));
			}
		}
	}
}

int ParseFen(char *fen, S_BOARD *pos) {

	ASSERT(fen!=NULL);
	ASSERT(pos!=NULL);

	int  rank = RANK_8;
    int  file = FILE_A;
    int  piece = 0;
    int  count = 0;
    int  i = 0;
	int  sq64 = 0;
	int  sq120 = 0;

	ResetBoard(pos);

	while ((rank >= RANK_1) && *fen) {
	    count = 1;
		switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;

            default:
                printf("FEN error \n");
                return -1;
        }

		for (i = 0; i < count; i++) {
            sq64 = rank * 8 + file;
			sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
			file++;
        }
		fen++;
	}

	ASSERT(*fen == 'w' || *fen == 'b');

	pos->side = (*fen == 'w') ? WHITE : BLACK;
	fen += 2;

	for (i = 0; i < 4; i++) {
        if (*fen == ' ') {
            break;
        }
		switch(*fen) {
			case 'K': pos->castlePerm |= WKCA; break;
			case 'Q': pos->castlePerm |= WQCA; break;
			case 'k': pos->castlePerm |= BKCA; break;
			case 'q': pos->castlePerm |= BQCA; break;
			default:	     break;
        }
		fen++;
	}
	fen++;

	ASSERT(pos->castlePerm>=0 && pos->castlePerm <= 15);

	if (*fen != '-') {
		file = fen[0] - 'a';
		rank = fen[1] - '1';

		ASSERT(file>=FILE_A && file <= FILE_H);
		ASSERT(rank>=RANK_1 && rank <= RANK_8);

		pos->enPas = FR2SQ(file,rank);
    }

	pos->posKey = GeneratePosKey(pos);

	UpdateListsMaterial(pos);

	return 0;
}

void ResetBoard(S_BOARD *pos) {
    int index = 0;

    for(index = 0; index < BRD_SQ_NUM; ++index){ //looping through all 120 squares
        pos->pieces[index] = OFFBOARD; // and setting them to "OFFBOARD"
    } //for more understanding, look at the image in the folder named "board representation"

    for(index = 0; index < 64; ++index) {
        pos->pieces[SQ120(index)] = EMPTY; // then the "real" squares are set to "empty"
    }

    for(index = 0; index < 2; ++index) {
        pos->bigPce[index] = 0; // number of all pieces (black and white)
        pos->majPce[index] = 0;
        pos->minPce[index] = 0;
        pos->material[index] = 0; //set material scores to 0
    }

    for(index = 0; index < 3; index++) {
        pos->pawns[index] = 0ULL; // OULL cuz pawns are an U64 type in an unsigned long long
    }

    for(index = 0; index < 13; ++index) {
        pos->pceNum[index] = 0; //resets the piece number
    }

    pos->KingSq[WHITE] = pos->KingSq[BLACK] = NO_SQ;

    pos->side = BOTH; //once we setup a position, checkboard will check some things so we set it to both not to get an ASSERT
    pos->enPas = NO_SQ; 
    pos->fiftyMove = 0;

    pos->ply = 0; //nber of half moves played in the current search
    pos->hisPly = 0;

    pos->castlePerm = 0;

    pos->posKey = 0ULL;
    printf("avant pvtable");
    InitPvTable(pos->PvTable);


}

void PrintBoard(const S_BOARD *pos) {


    int sq,file,rank,piece; 

	printf("\nGame Board:\n\n"); 

	for(rank = RANK_8; rank >= RANK_1; rank--) { // stat on the 8th rank cuz its how we look at it
		printf("%d  ",rank+1);
		for(file = FILE_A; file <= FILE_H; file++) {
			sq = FR2SQ(file,rank); //sqare index
			piece = pos->pieces[sq]; // get the piece using the square index from board
			printf("%3c",PceChar[piece]);
		}
		printf("\n");
	}

	printf("\n   "); //just showing files and rank
	for(file = FILE_A; file <= FILE_H; file++) {
		printf("%3c",'a'+file);
	}
	printf("\n");
	printf("side:%c\n",SideChar[pos->side]);
	printf("enPas:%d\n",pos->enPas); //printed as a decimal rather then as characters
	printf("castle:%c%c%c%c\n",
			pos->castlePerm & WKCA ? 'K' : '-', //if castle perm ends with ' ' then print the corresponding letters
			pos->castlePerm & WQCA ? 'Q' : '-',
			pos->castlePerm & BKCA ? 'k' : '-',
			pos->castlePerm & BQCA ? 'q' : '-'
			);
	printf("PosKey:%llX\n",pos->posKey); //hexa cuz easier to read


}


/* ========================================== */
/* FICHIER : data.c */
/* ========================================== */

#include "def.h"

char PceChar[] = ".PNBRQKpnbrqk"; //piece according the number
char SideChar[] = "wb-"; //self explanatory '-' is for both b and w
char RankChar[] = "12345678"; //for ranks
char FileChar[] = "abcdefgh"; //for files


int PieceBig[13] = { FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE }; //knigh bishop rook queen king are big pieces
int PieceMaj[13] = { FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE }; // king queen rook (black and white)
int PieceMin[13] = { FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE };
int PieceVal[13]= { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000  }; // pawn worth 100 and 325 for bishop and knight rather than 300 cuz "oldschool"
int PieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
	BLACK, BLACK, BLACK, BLACK, BLACK, BLACK }; //gives colo of a piece


int PiecePawn[13] = { FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE };	
int PieceKnight[13] = { FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE };
int PieceKing[13] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE };
int PieceRookQueen[13] = { FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE };
int PieceBishopQueen[13] = { FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE };
int PieceSlides[13] = { FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE };

// int Mirror64[64] = {
// 56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
// 48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
// 40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
// 32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
// 24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
// 16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
// 8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
// 0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
// };





/* ========================================== */
/* FICHIER : evaluate.c */
/* ========================================== */


#include <stdio.h>
#include "def.h"

const int PawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int KnightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

const int BishopTable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int RookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

const int Mirror64[64] = {
56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
0	,	1	,	2	,	3	,	4	,	5	,	6	,	7		
};

#define MIRROR64(sq) (Mirror64[(sq)])

int EvalPosition(const S_BOARD *pos) {

    int pce;
	int pceNum;
	int sq;
	int score = pos->material[WHITE] - pos->material[BLACK];

    pce = wP;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += PawnTable[SQ64(sq)];	
		
	}	

	pce = bP;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= PawnTable[MIRROR64(SQ64(sq))];	
		  
	}	

    pce = wN;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += KnightTable[SQ64(sq)];
	}	

	pce = bN;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= KnightTable[MIRROR64(SQ64(sq))];
	}			
	
	pce = wB;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += BishopTable[SQ64(sq)];
	}	

	pce = bB;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= BishopTable[MIRROR64(SQ64(sq))];
	}	

	pce = wR;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += RookTable[SQ64(sq)];
	}	

	pce = bR;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= RookTable[MIRROR64(SQ64(sq))];

	}	
    return 0;
}


/* ========================================== */
/* FICHIER : init.c */
/* ========================================== */

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


/* ========================================== */
/* FICHIER : misc.c */
/* ========================================== */

//misc.c

#include <stdio.h>
#include "def.h"


#ifdef WIN32
#include "windows.h"
#else
#include "sys/time.h"
// #include "sys/select.h"
#include "unistd.h"
#include "string.h"
#endif

int GetTimeMs() {
#ifdef WIN32
  return GetTickCount();
#else
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec*1000 + t.tv_usec/1000;
#endif
}




/* ========================================== */
/* FICHIER : perft.c */
/* ========================================== */

// perft.c

#include <stdio.h>
#include "def.h"
 //self explanatory

long leafNodes;

void Perft(int depth, S_BOARD *pos) {

    ASSERT(CheckBoard(pos));  

	if(depth == 0) {
        leafNodes++;
        return;
    }	

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
      
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
       
        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        Perft(depth - 1, pos);
        TakeMove(pos);
    }

    return;
}


void PerftTest(int depth, S_BOARD *pos) { //kindof a mirror of the perft function

    ASSERT(CheckBoard(pos));

	PrintBoard(pos);
	printf("\nStarting Test To Depth:%d\n",depth);	
	leafNodes = 0;
	int start = GetTimeMs();
    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);	
    
    int move;	    
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        move = list->moves[MoveNum].move;
        if ( !MakeMove(pos,move))  {
            continue;
        }
        long cumnodes = leafNodes; // creaztze a cumulative node variables on each loop
        Perft(depth - 1, pos);
        TakeMove(pos);        
        long oldnodes = leafNodes - cumnodes;
        printf("move %d : %s : %ld\n",MoveNum+1,PrMove(move),oldnodes);
    }   
	
	printf("\nTest Complete : %ld nodes visited in %dms\n",leafNodes,GetTimeMs() - start);
    //just shows all the leafs possible
    return;
}


/* ========================================== */
/* FICHIER : search.c */
/* ========================================== */

#include "def.h"
#include <stdio.h>

static void CheckUp(S_SEARCHINFO *info) { //will be called every 4k node or so
	// .. check if time up, or interrupt from GUI
	if(info->timeset == TRUE && GetTimeMs() > info->stoptime) {
		info->stopped = TRUE;
	}

	// ReadInput(info);
}

static void PickNextMove(int moveNum, S_MOVELIST *list) {

	S_MOVE temp;
	int index = 0;
	int bestScore = 0;
	int bestNum = moveNum;

	for (index = moveNum; index < list->count; ++index) {
		if (list->moves[index].score > bestScore) {
			bestScore = list->moves[index].score;
			bestNum = index;
		}
	}

	ASSERT(moveNum>=0 && moveNum<list->count);
	ASSERT(bestNum>=0 && bestNum<list->count);
	ASSERT(bestNum>=moveNum);

	temp = list->moves[moveNum];
	list->moves[moveNum] = list->moves[bestNum];
	list->moves[bestNum] = temp;
}


static int IsRepetition(const S_BOARD *pos) { //self explanaytory, detects if we repeted the position

	int index = 0;
    //just starts counting not at zero but at the last time the fifty move rule has been reset
	for(index = pos->hisPly - pos->fiftyMove; index < pos->hisPly-1; ++index) {
		ASSERT(index >= 0 && index < MAXGAMEMOVES);
		if(pos->posKey == pos->history[index].posKey) {
			return TRUE;
		}
	}
	return FALSE;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
//clear the various stats of historic array and clear all to zero 
//and set it ready for a new search
	int index = 0;
	int index2 = 0;

	for(index = 0; index < 13; ++index) {
		for(index2 = 0; index2 < BRD_SQ_NUM; ++index2) {
			pos->searchHistory[index][index2] = 0;
		}
	}

	for(index = 0; index < 2; ++index) {
		for(index2 = 0; index2 < MAXDEPTH; ++index2) {
			pos->searchKillers[index][index2] = 0;
		}
	}

	ClearPvTable(pos->PvTable);
	pos->ply = 0;

	info->starttime = GetTimeMs();
	info->stopped = 0;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	ASSERT(CheckBoard(pos));
	if((info->nodes & 2047) == 0) {
		CheckUp(info);
	}
	info->nodes++;

	if(IsRepetition(pos) || pos->fiftyMove >= 100) {
		return 0;
	}
	if(pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}

	int Score = EvalPosition(pos);

	if(Score >= beta) {
		return beta;
	}

	if(Score > alpha) {
		alpha = Score;
	}

	S_MOVELIST list[1];
	GenerateAllCaps(pos,list);

	int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	Score = -INFINITE; 
	int PvMove = ProbePvTable(pos);

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

		PickNextMove(MoveNum, list);
		if( !MakeMove(pos, list->moves[MoveNum].move)) {
			continue;
		}
 
		Legal++;
		Score = -Quiescence(-beta, -alpha, pos, info);
		TakeMove(pos);

		if(info->stopped == TRUE) {
			return 0;
		}

		if(Score > alpha) {
			if(Score >= beta) {
				if(Legal==1) {
					info->fhf++;
				}
				info->fh++;
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;
		}
	}

	if(alpha != OldAlpha) {
		StorePvMove(pos,BestMove );
	}
	return alpha;
}


static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {

	int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	int Score = -INFINITE; 
	int PvMove = ProbePvTable(pos);
	ASSERT(CheckBoard(pos));


	if(depth == 0) {
		return Quiescence(alpha, beta, pos, info);
	}

	if((info->nodes & 2047) == 0) {
		CheckUp(info);
	}

	info->nodes++;	
	if(IsRepetition(pos) || pos->fiftyMove >= 100) { //its a draw
		return 0;
	}

	if(pos->ply > MAXDEPTH - 1) { //then we've gone too deep
		return EvalPosition(pos);
	}

	S_MOVELIST list[1];
    GenerateAllMoves(pos,list);

    

	if (PvMove != NOMOVE) {
		for(MoveNum = 0; MoveNum < list->count; MoveNum++) {
			if (list->moves[MoveNum].move == PvMove) {
				list->moves[MoveNum].score = 2000000;
				break;
			}
		}
	}

		
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

		PickNextMove(MoveNum, list);
		if( !MakeMove(pos, list->moves[MoveNum].move)) {
			continue;
		}

		Legal++;
		Score = -AlphaBeta(-beta, -alpha, depth-1, pos, info, TRUE);
		TakeMove(pos);

		if(info->stopped == TRUE) {
			return 0;
		}
		if(Score > alpha) {
			if(Score >= beta) {
				if(Legal==1) {
					info->fhf++;
				}
				info->fh++;

				if(!(list->moves[MoveNum].move && MFLAGCAP)) {
					pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
					pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
				}
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;
			if(!(list->moves[MoveNum].move & MFLAGCAP)) {
				pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
			}
		}

	}

	if(Legal == 0) {
		if(SqAttacked(pos->KingSq[pos->side], pos->side^1, pos)) {
			return -MATE + pos->ply;
		} else {
			return 0;
		}
	}

	if(alpha != OldAlpha) {
		StorePvMove(pos, BestMove);
	}

	return alpha;
}



void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
//iterative deepening (alpha beta...)
// for depth = 1 to maxdepth
	//search alpha beta....
	//next depth

	int bestMove = NOMOVE;
	int bestScore = -INFINITE;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNum = 0;
	ClearForSearch(pos,info);

	for( currentDepth = 1; currentDepth <= info->depth; ++currentDepth ) {
								//alpha		//beta
		bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);

		if(info->stopped == TRUE) {
			break;
		}
		pvMoves = GetPvLine(currentDepth, pos);
		bestMove = pos->PvArray[0];
		printf("Depth:%d score:%d move:%s nodes:%ld", currentDepth, bestScore, PrMove(bestMove), info->nodes);


		pvMoves = GetPvLine(currentDepth, pos);
		printf("pv");
		for(pvNum = 0; pvNum<pvMoves; pvNum++) {
			printf(" %s", PrMove(pos->PvArray[pvNum]));
		}
		printf("\n");
		printf("Ordering:%.2f\n", (info->fhf/info->fh));
	}
}





/* ========================================== */
/* FICHIER : bitboards.c */
/* ========================================== */

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


/* ========================================== */
/* FICHIER : def.h */
/* ========================================== */

#ifndef def_h
#define def_h

#include <stdlib.h>

// #define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(! (n)) { \
    printf("%s - FAILED ", #n); \
    printf("on %s ", __DATE__); \
    printf("at %s ", __TIME__); \
    printf("In file %s ", __FILE__); \
    printf("At line %d\n", __LINE__); \
    exit(1);}

#endif

typedef unsigned long long U64;

#define NAME "senzu 1.0"
#define BRD_SQ_NUM 120   // cuz 120 cases on the board (0 to 120)

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define MAXGAMEMOVES 2048 // number max of moves in a game (will never never never be more than 2048 exept if u trynna beat a world record)
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INFINITE 30000
#define MATE (INFINITE - MAXDEPTH)


enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK};
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum { WHITE, BLACK, BOTH };

enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

enum { FALSE, TRUE };

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8}; // white king castle .... (long or short castle)
                                                // if a rooq is moved, we cannot castle with it anymore so BQCA or BKCA won't be possible



typedef struct {
	int move;
	int score;
} S_MOVE;

typedef struct {
    S_MOVE moves[MAXPOSITIONMOVES];
    int count;
} S_MOVELIST;

typedef struct {
    U64 posKey;
    int move;
} S_PVENTRY;

typedef struct {
    S_PVENTRY *pTable;
    int numEntries;
} S_PVTABLE;

typedef struct{ // to store the history to undo a move if it wasn't legal
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey; //or hashkey its the same
} S_UNDO;

typedef struct {

    int pieces[BRD_SQ_NUM];
    U64 pawns[3]; // 0100000 for the first line means we have a pawn on B1

    int KingSq[2]; // same for kings


    int side;
    int enPas;
    int fiftyMove;

    int ply;
    int hisPly;

    int castlePerm; // will be represented by 4 bits 0(WKCA) 0(WQCA) 0(BKCA) 0(BQCA) so the castle permission
                    // if we have  1 0 0 1, we can castle king side for whites & queen side for blacks

    U64 posKey; //or hashkey again will be used to represent the position of the board

    int pceNum[13]; // number of different pieces on the board ( pawn, bishop, rooq, knicght, queen, king) x2 for black and white and then a empty case
    int bigPce[3]; // number of "big" pieces (everything that's not a pawn)
    int majPce[3]; // major pieces (queen and rooqs)
    int minPce[3]; // minor pieces (knight and bishop)
    int material[2];

    S_UNDO history[MAXGAMEMOVES]; //  where we will store all the moves made dyring the game

    // piece list
    /*we could loop on the entiere board until we come across avery piece and genereate all the moves possible
    but its not very fast, so we do a piece list*/ 

    int pList[13][10]; // 13 different pieeces which u can have maximum 10 each (like promoting all ur pawns to roks and u get 10 rooks)

    S_PVTABLE PvTable[1];

    int PvArray[MAXDEPTH];
    // pList[wN][0] = E1; so there's a whute knight on e1
    // pList[wN][1] = D4; ...... u til we don't get anymore knichgts added
    // this is ~20% faster according to a guy's test 

    int searchHistory[13][BRD_SQ_NUM];

	int searchKillers[2][MAXDEPTH];


} S_BOARD;

typedef struct { //usefull when we'll search and print some things at the screen

	int starttime;
	int stoptime;
	int depth;
	int timeset;
	int movestogo;

	long nodes;

	int quit;
	int stopped;

	float fh;
	float fhf;
	int nullCut;

	int GAME_MODE;
	int POST_THINKING;

} S_SEARCHINFO;

// its kinda complicated to set bitwise the en passant so it's not really explained here
// if u have perfect knowledge of the way i defined the whole board and movements, it's easy to understand but i can't really explain it

#define FROMSQ(m) ((m) & 0x7F) 
#define TOSQ(m) (((m)>>7) & 0x7F) // for the move
#define CAPTURED(m) (((m)>>14) & 0xF) // the piece captured in a move
#define PROMOTED(m) (((m)>>20) & 0xF) // self explanatory

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000 // if anything got captured 
#define MFLAGPROM 0xF00000

#define NOMOVE 0

/* MACROS */

#define FR2SQ(f,r) ( (21 + (f) ) + ( (r) * 10) ) // for a given file (f) and rank (r) returns the equivalent square in the 120 square 2D array
#define SQ64(sq120) (Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define CLRBIT(bb,sq)  ((bb) &= ClearMask[(sq)])
#define SETBIT(bb,sq)  ((bb) |= SetMask[(sq)])


#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsKn(p) (PieceKnight[(p)])
#define IsKi(p) (PieceKing[(p)])


/* GLOBALS */

extern int Sq120ToSq64[BRD_SQ_NUM]; // will convert the index from the 120 squares board (with the grey things) to the "real" board of 64 cases
extern int Sq64ToSq120[64]; // literally the opposite of the previous one (square 64 to square 120)
extern U64 SetMask[64]; //to set a bit on the bitboard (set it from 0 to 1)
extern U64 ClearMask[64];  //to clear a it from the bitboard (from 1 to 0)
extern U64 PieceKeys[13][120]; //#pieces index and each sqaure
extern U64 SideKey; //hashing in a random number if its white to move
// 0 0 0 0
extern U64 CastleKeys[16]; // 16 castle cases cuz bits are worth 1+2+4+8 = 16
extern char PceChar[];
extern char SideChar[]; //all 4 indexed using the corresponding integer to be able to print the piece side and rnak and file
extern char RankChar[];
extern char FileChar[];

extern int PieceBig[13];
extern int PieceMaj[13];
extern int PieceMin[13];
extern int PieceVal[13];
extern int PieceCol[13];
extern int PiecePawn[13];

extern int FilesBrd[BRD_SQ_NUM];
extern int RanksBrd[BRD_SQ_NUM];

extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];
extern int PieceSLides[13];

/* FUNCTIONS */


// init.c
extern void AllInit();

// bitboards.c
extern void PrintBitBoard(U64 bb);
extern int PopBit(U64 *bb);
extern int CountBits(U64 b);

// hashkeys.c
extern U64 GeneratePosKey(const S_BOARD *pos);

// board.c
extern void ResetBoard(S_BOARD *pos);
extern int ParseMove(char *ptrChar, S_BOARD *pos);
extern void PrintBoard(const S_BOARD *pos);
extern void UpdateListsMaterial(S_BOARD *pos);
extern int CheckBoard(const S_BOARD *pos);
extern void MirrorBoard(S_BOARD *pos);
int ParseFen(char *fen, S_BOARD *pos);

// attack.c
extern int SqAttacked(const int sq, const int side, const S_BOARD *pos);

// io.c
extern char *PrMove(const int move);
extern char *PrSq(const int sq);
extern void PrintMoveList(const S_MOVELIST *list);
extern int ParseMove(char *ptrChar, S_BOARD *pos);


//validate.c
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);
extern void MirrorEvalTest(S_BOARD *pos);
extern int SqIs120(const int sq);
extern int PceValidEmptyOffbrd(const int pce);
extern int MoveListOk(const S_MOVELIST *list,  const S_BOARD *pos);
extern void DebugAnalysisTest(S_BOARD *pos, S_SEARCHINFO *info);

// movegen.c
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);
extern void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list);
extern int MoveExists(S_BOARD *pos, const int move);
extern void InitMvvLva();

// makemove.c
extern int MakeMove(S_BOARD *pos, int move);
extern void TakeMove(S_BOARD *pos);
extern void MakeNullMove(S_BOARD *pos);
extern void TakeNullMove(S_BOARD *pos);

// perft.c
extern void PerftTest(int depth, S_BOARD *pos);

// search.c
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);

// misc.c
extern int GetTimeMs();
extern void ReadInput(S_SEARCHINFO *info);

// pvtable.c

extern void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);
extern int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);
extern int ProbePvMove(const S_BOARD *pos);
extern int GetPvLine(const int depth, S_BOARD *pos);
extern void InitPvTable(S_PVTABLE *table);
extern void ClearPvTable(S_PVTABLE *table);
extern void StorePvMove(const S_BOARD *pos, const int move);
extern int ProbePvTable(const S_BOARD *pos);

// evaluate.c
extern int EvalPosition(const S_BOARD *pos);
extern void MirrorEvalTest(S_BOARD *pos) ;

// uci.c
extern void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info);

// xboard.c
extern void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info);
extern void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info);

// polybook.c
extern int GetBookMove(S_BOARD *board);
extern void CleanPolyBook();
extern void InitPolyBook() ;

#endif


/* ========================================== */
/* FICHIER : hashkeys.c */
/* ========================================== */

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


/* ========================================== */
/* FICHIER : io.c */
/* ========================================== */



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



/* ========================================== */
/* FICHIER : makemove.c */
/* ========================================== */

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



/* ========================================== */
/* FICHIER : movegen.c */
/* ========================================== */


#include <stdio.h>
#include "def.h"



/* ----------- whole principle of this file --------------
MoveGen(board, lsit) {
    loop all pieces ->
        -> slider  loop each dir add move
            -> addmove list->moves[list->count] = move; list->count++;

}
*/
#define MOVE(f,t,ca,pro,fl) ( (f) | ((t) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)]==OFFBOARD)

const int LoopSlidePce[8] = {
 wB, wR, wQ, 0, bB, bR, bQ, 0
};

const int LoopNonSlidePce[6] = {
 wN, wK, 0, bN, bK, 0
};

const int LoopSlideIndex[2] = { 0, 4 };
const int LoopNonSlideIndex[2] = { 0, 3 };

const int PceDir[13][8] = { //what the increments for each piece are
	{ 0, 0, 0, 0, 0, 0, 0 }, //nomove
	{ 0, 0, 0, 0, 0, 0, 0 }, // promotions
	{ -8, -19,	-21, -12, 8, 19, 21, 12 }, // L shaped (knight)
	{ -9, -11, 11, 9, 0, 0, 0, 0 }, // diagonal moves
	{ -1, -10,	1, 10, 0, 0, 0, 0 }, //straight lines
	{ -1, -10,	1, 10, -9, -11, 11, 9 }, //bishop + rook = queen
	{ -1, -10,	1, 10, -9, -11, 11, 9 }, // queen moves but only one step = king
	{ 0, 0, 0, 0, 0, 0, 0 }, //promotions
	{ -8, -19,	-21, -12, 8, 19, 21, 12 }, //and same for other side
	{ -9, -11, 11, 9, 0, 0, 0, 0 }, 
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};

const int NumDir[13] = { // how many directions do thye have
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

/*
PV Move
Cap -> MvvLVA
Killers
HistoryScore

*/
const int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };
static int MvvLvaScores[13][13];

/*
PV move
cap->mvvLVA (most valuable victim, less valuable atacker)
so we sort an array like this [vic][att]
and then we want to list all the moves starting from the less valuable piece attacking the most valuable piece (pawn takes queen) 
until queen takes pawn
we'll giuve each move a score

function written down there
*/

void InitMvvLva() {
	int Attacker;
	int Victim;
	for(Attacker = wP; Attacker <= bK; ++Attacker) {
		for(Victim = wP; Victim <= bK; ++Victim) {
			MvvLvaScores[Victim][Attacker] = VictimScore[Victim] + 6 - ( VictimScore[Attacker] / 100);
		}
	}
}

int MoveExists(S_BOARD *pos, const int move) {
// the board is a const cuz we'll generatee all the moves on the board and then make the moves 
	S_MOVELIST list[1];
    GenerateAllMoves(pos,list);

    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) { //loop through all the moves

        if ( !MakeMove(pos,list->moves[MoveNum].move))  { // if the move isnt legal, we continue
            continue;
        }
        TakeMove(pos); // if it is legal we take it back and we check if the move is egal to the move in argument
		if(list->moves[MoveNum].move == move) {
			return TRUE; //then we know the move exists
		}
    }
	return FALSE;
}

static void AddQuietMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {

	ASSERT(SqOnBoard(FROMSQ(move)));
	ASSERT(SqOnBoard(TOSQ(move)));
	ASSERT(CheckBoard(pos));
	ASSERT(pos->ply >=0 && pos->ply < MAXDEPTH);

	list->moves[list->count].move = move;

	if(pos->searchKillers[0][pos->ply] == move) {
		list->moves[list->count].score = 900000;
	} else if(pos->searchKillers[1][pos->ply] == move) {
		list->moves[list->count].score = 800000;
	} else {
		list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
	}
	list->count++;
}

static void AddCaptureMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {

	ASSERT(SqOnBoard(FROMSQ(move)));
	ASSERT(SqOnBoard(TOSQ(move)));
	ASSERT(PieceValid(CAPTURED(move)));
	ASSERT(CheckBoard(pos));

	list->moves[list->count].move = move;
	list->moves[list->count].score = MvvLvaScores[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1000000;
	list->count++;
}

static void AddEnPassantMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {

	ASSERT(SqOnBoard(FROMSQ(move)));
	ASSERT(SqOnBoard(TOSQ(move)));
	ASSERT(CheckBoard(pos));
	ASSERT((RanksBrd[TOSQ(move)]==RANK_6 && pos->side == WHITE) || (RanksBrd[TOSQ(move)]==RANK_3 && pos->side == BLACK));

	list->moves[list->count].move = move;
	list->moves[list->count].score = 105 + 1000000;
	list->count++;
}

static void AddWhitePawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) {

	ASSERT(PieceValidEmpty(cap));
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(CheckBoard(pos));

	if(RanksBrd[from] == RANK_7) {
		AddCaptureMove(pos, MOVE(from,to,cap,wQ,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wR,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wB,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wN,0), list);
	} else {
		AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
	}
}

static void AddWhitePawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list ) {

	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(CheckBoard(pos));

	if(RanksBrd[from] == RANK_7) {
		AddQuietMove(pos, MOVE(from,to,EMPTY,wQ,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,wR,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,wB,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,wN,0), list);
	} else {
		AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
	}
}

static void AddBlackPawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) {

	ASSERT(PieceValidEmpty(cap));
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(CheckBoard(pos));

	if(RanksBrd[from] == RANK_2) {
		AddCaptureMove(pos, MOVE(from,to,cap,bQ,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bR,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bB,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bN,0), list);
	} else {
		AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
	}
}

static void AddBlackPawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list ) {

	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(CheckBoard(pos));

	if(RanksBrd[from] == RANK_2) {
		AddQuietMove(pos, MOVE(from,to,EMPTY,bQ,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,bR,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,bB,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,bN,0), list);
	} else {
		AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
	}
}

void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list) {

	ASSERT(CheckBoard(pos));

	list->count = 0;

	int pce = EMPTY;
	int side = pos->side;
	int sq = 0; int t_sq = 0;
	int pceNum = 0;
	int dir = 0;
	int index = 0;
	int pceIndex = 0;

	if(side == WHITE) {

		for(pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
			sq = pos->pList[wP][pceNum];
			ASSERT(SqOnBoard(sq));

			if(pos->pieces[sq + 10] == EMPTY) {
				AddWhitePawnMove(pos, sq, sq+10, list);
				if(RanksBrd[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
					AddQuietMove(pos, MOVE(sq,(sq+20),EMPTY,EMPTY,MFLAGPS),list);
				}
			}

			if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], list);
			}
			if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], list);
			}

			if(pos->enPas != NO_SQ) {
				if(sq + 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEP), list);
				}
				if(sq + 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEP), list);
				}
			}
		}

		if(pos->castlePerm & WKCA) {
			if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
				if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(F1,BLACK,pos) ) {
					AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}

		if(pos->castlePerm & WQCA) {
			if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
				if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(D1,BLACK,pos) ) {
					AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}

	} else {

		for(pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
			sq = pos->pList[bP][pceNum];
			ASSERT(SqOnBoard(sq));

			if(pos->pieces[sq - 10] == EMPTY) {
				AddBlackPawnMove(pos, sq, sq-10, list);
				if(RanksBrd[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
					AddQuietMove(pos, MOVE(sq,(sq-20),EMPTY,EMPTY,MFLAGPS),list);
				}
			}

			if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
			}

			if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
			}
			if(pos->enPas != NO_SQ) {
				if(sq - 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEP), list);
				}
				if(sq - 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEP), list);
				}
			}
		}

		// castling
		if(pos->castlePerm &  BKCA) {
			if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
				if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(F8,WHITE,pos) ) {
					AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}

		if(pos->castlePerm &  BQCA) {
			if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
				if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(D8,WHITE,pos) ) {
					AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}
	}

	/* Loop for slide pieces */
	pceIndex = LoopSlideIndex[side];
	pce = LoopSlidePce[pceIndex++];
	while( pce != 0) {
		ASSERT(PieceValid(pce));

		for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
			sq = pos->pList[pce][pceNum];
			ASSERT(SqOnBoard(sq));

			for(index = 0; index < NumDir[pce]; ++index) {
				dir = PceDir[pce][index];
				t_sq = sq + dir;

				while(!SQOFFBOARD(t_sq)) {
					// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
					if(pos->pieces[t_sq] != EMPTY) {
						if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
							AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
						}
						break;
					}
					AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
					t_sq += dir;
				}
			}
		}

		pce = LoopSlidePce[pceIndex++];
	}

	/* Loop for non slide */
	pceIndex = LoopNonSlideIndex[side];
	pce = LoopNonSlidePce[pceIndex++];

	while( pce != 0) {
		ASSERT(PieceValid(pce));

		for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
			sq = pos->pList[pce][pceNum];
			ASSERT(SqOnBoard(sq));

			for(index = 0; index < NumDir[pce]; ++index) {
				dir = PceDir[pce][index];
				t_sq = sq + dir;

				if(SQOFFBOARD(t_sq)) {
					continue;
				}

				// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
				if(pos->pieces[t_sq] != EMPTY) {
					if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					continue;
				}
				AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
			}
		}

		pce = LoopNonSlidePce[pceIndex++];
	}

    // ASSERT(MoveListOk(list,pos));
}


void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list) {

	ASSERT(CheckBoard(pos));

	list->count = 0;

	int pce = EMPTY;
	int side = pos->side;
	int sq = 0; int t_sq = 0;
	int pceNum = 0;
	int dir = 0;
	int index = 0;
	int pceIndex = 0;

	if(side == WHITE) {

		for(pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
			sq = pos->pList[wP][pceNum];
			ASSERT(SqOnBoard(sq));

			if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], list);
			}
			if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], list);
			}

			if(pos->enPas != NO_SQ) {
				if(sq + 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEP), list);
				}
				if(sq + 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEP), list);
				}
			}
		}

	} else {

		for(pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
			sq = pos->pList[bP][pceNum];
			ASSERT(SqOnBoard(sq));

			if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
			}

			if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
			}
			if(pos->enPas != NO_SQ) {
				if(sq - 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEP), list);
				}
				if(sq - 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEP), list);
				}
			}
		}
	}

	/* Loop for slide pieces */
	pceIndex = LoopSlideIndex[side];
	pce = LoopSlidePce[pceIndex++];
	while( pce != 0) {
		ASSERT(PieceValid(pce));

		for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
			sq = pos->pList[pce][pceNum];
			ASSERT(SqOnBoard(sq));

			for(index = 0; index < NumDir[pce]; ++index) {
				dir = PceDir[pce][index];
				t_sq = sq + dir;

				while(!SQOFFBOARD(t_sq)) {
					// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
					if(pos->pieces[t_sq] != EMPTY) {
						if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
							AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
						}
						break;
					}
					t_sq += dir;
				}
			}
		}

		pce = LoopSlidePce[pceIndex++];
	}

	/* Loop for non slide */
	pceIndex = LoopNonSlideIndex[side];
	pce = LoopNonSlidePce[pceIndex++];

	while( pce != 0) {
		ASSERT(PieceValid(pce));

		for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
			sq = pos->pList[pce][pceNum];
			ASSERT(SqOnBoard(sq));

			for(index = 0; index < NumDir[pce]; ++index) {
				dir = PceDir[pce][index];
				t_sq = sq + dir;

				if(SQOFFBOARD(t_sq)) {
					continue;
				}

				// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
				if(pos->pieces[t_sq] != EMPTY) {
					if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					continue;
				}
			}
		}

		pce = LoopNonSlidePce[pceIndex++];
	}
    // ASSERT(MoveListOk(list,pos));
}





/* ========================================== */
/* FICHIER : pvtable.c */
/* ========================================== */

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



/* ========================================== */
/* FICHIER : senzu.c */
/* ========================================== */

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




/* ========================================== */
/* FICHIER : validate.c */
/* ========================================== */


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



