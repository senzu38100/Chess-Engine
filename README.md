# Chess Engine (C)

Chess engine written in C, based on bitboards and the UCI protocol.  
The project can generate legal moves, evaluate positions, and play games through any UCI-compatible interface.

## Features
- Bitboard-based board representation
- Legal move generation
- Basic position evaluation
- Search (minimax / alpha-beta)
- Hash tables (PV table)
- UCI protocol support
- Testing tools (perft)

## Project Structure
- `Board.c` : board management
- `bitboards.c` : bitboard operations
- `movegen.c` : move generation
- `makemove.c` : move execution
- `evaluate.c` : evaluation function
- `perft.c` : performance testing
- `io.c` : UCI input/output
- `def.h` : global definitions
- `Makefile` : build configuration

## Build
```bash
make
