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