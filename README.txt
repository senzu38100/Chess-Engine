Chess AI - README

Overview

This Chess AI is a custom-built chess engine designed to play chess efficiently while showcasing various algorithmic techniques. It is compatible with both Winboard and UCI protocols, making it usable with GUI interfaces such as Shredder, Winboard, Chessbase, and Arena. The engine also supports a console mode for direct interaction.

Features

Alpha-Beta Search - Optimized search algorithm for decision-making.

Iterative Deepening - Improves move selection depth dynamically.

Quiescence Search - Reduces horizon effect by extending critical positions.

Transposition Table - Caches positions to avoid redundant calculations.

Always Replace Scheme - Implements an efficient way to store positions.

Polyglot Opening Books - Supports pre-defined opening strategies.

MVV/LVA Move Ordering - Prioritizes captures for better search efficiency.

Basic Evaluation - Assigns weights to material and position.

Special Chess Rules Implemented

Castling - Ensures the king and rook move correctly under the right conditions.

En Passant - Allows for special pawn captures.

50-Move Rule - Tracks the move count to claim a draw.

FEN Notation Support - Parses and generates FEN strings (Video 14 reference).

Move History Tracking - Maintains past moves and allows for undoing moves.

Move Execution Logic

The logic behind making a move follows these steps:

Extract from, to, and capture information from the move.

Store the current position.

Move the current piece from from -> to.

If a capture is made, remove the captured piece from the list.

Check for the fifty-move rule condition.

Handle promotions if a pawn reaches the last rank.

Handle En Passant captures.

Update En Passant square if applicable.

Update piece lists for additions, removals, and moves.

Maintain hash key to speed up position lookup.

Update castling permissions.

Generate the new FEN position.

Change side and increment game ply counters.

Installation & Usage

Download the latest release of the Chess AI.

Run the executable in console mode or link it to a compatible GUI.

Optionally, load a Polyglot Opening Book for improved openings.

License & Contributions

You are welcome to use, modify, and improve this project. Contributions and enhancements are encouraged!

Developed as part of a chess AI learning project. Inspired by Vice (Video Instructional Chess Engine).