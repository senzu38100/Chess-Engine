all:
	gcc senzu.c init.c misc.c bitboards.c search.c perft.c makemove.c validate.c pvtable.c  hashkeys.c board.c movegen.c attack.c io.c data.c evaluate.c -o senzu