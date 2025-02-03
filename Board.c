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