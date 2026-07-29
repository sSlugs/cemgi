#include "movegen/movegen.h"
#include "board/board.h"
#include "movegen/move.h"
#include "globals.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include <stdio.h>

void PseudoQuietKnightGen(Board *board, MoveList *movelist) {
    u64 knights = board->pieces[board->turn][Knight];

    // iterate through all knights of current turn's colour
    while (knights != 0) {
	// gets index 0-63 by finding LSB so we can avoid long for loop
	int source = LsbIndex(knights);

	// since its quiet get dest using attack mask & inverse of all occupancy which ignores captures
	u64 dest_bb = KNIGHT_ATTACKS[source] & ~board->occupancy[ALL];

	// iterate through all the destinations
	while (dest_bb != 0) {
	    int dest = LsbIndex(dest_bb);

	    // its a quiet knight move so we dont need to add any flags
	    Move move = MoveNew(source,dest);

	    MoveListPush(movelist, move);

	    PopLsb(&dest_bb);
	}

	PopLsb(&knights);
    }
}

void PseudoCaptureKnightGen(Board *board, MoveList *movelist) {
    Colour colour = board->turn;
    u64 knights = board->pieces[colour][Knight];

    // iterate through all knights of current turn's colour
    while (knights != 0) {
	int source = LsbIndex(knights);

	// since its capture we only care about knight moves that land on enemy pieces
	u64 dest_bb = KNIGHT_ATTACKS[source] & board->occupancy[InverseColour(colour)];

	// iterate through all the destinations
	while (dest_bb != 0) {
	    int dest = LsbIndex(dest_bb);

	    // add capture flag
	    Move move = MoveNew(source,dest) | CAPTURE;

	    MoveListPush(movelist, move);

	    PopLsb(&dest_bb);
	}

	PopLsb(&knights);
    }
}

void PseudoQuietKingGen(Board *board, MoveList *movelist) {
    Colour colour = board->turn;

    int source = LsbIndex(board->pieces[colour][King]);

    // theres only gonna be 1 king so no need for while loop
    u64 dest_bb = KING_ATTACKS[source] & ~board->occupancy[ALL];

    // iterate through all the destinations
    while (dest_bb != 0) {
	int dest = LsbIndex(dest_bb);

	Move move = MoveNew(source,dest);

	MoveListPush(movelist, move);

	PopLsb(&dest_bb);
    }

    // Castle rights

    // since white = 0, black = 1, and castle rights in board are phyiscal bits in order WQC,WKC,BQC,BKC we can use this cool trick to shift to proper colour!
    int shift = 2*colour;

    // if we have castle kingside rights
    if (board->castle_rights & (WKC << shift)) {
	// check if squares are empty. because we know if we have castle rights kin will be in e4 or e8, shifting once and twice will give us the square king must pass through regardless of colour
	int ksq1 = (1ULL << (source + 1)), ksq2 = (1ULL << (source+2));
	if (!(board->occupancy[ALL] & ksq1) && !(board->occupancy[ALL] & ksq2)) {
	    // last check to see if ksq1 and ksq2 are currently under attack
	    if (!(SquareAttackedByColour(board, ksq1, InverseColour(colour))) && !(SquareAttackedByColour(board, ksq2, InverseColour(colour)))) {
		// if all checks pass we can push kingside castle
		MoveListPush(movelist, MoveNew(source, ksq2) | KING_CASTLE);
	    }
	}
    }

    // if we have castle queenside rights
    if (board->castle_rights & (WQC << shift)) {
	// check if squares are empty
	int ksq1 = (1ULL >> (source + 1)), ksq2 = (1ULL >> (source+2)),ksq3 = (1ULL >> (source+3));
	if (!(board->occupancy[ALL] & ksq1) && !(board->occupancy[ALL] & ksq2) && !(board->occupancy[ALL] & ksq3)) {
	    // last check to see if ksq1 and ksq2 are currently under attack
	    if (!(SquareAttackedByColour(board, ksq1, InverseColour(colour))) && !(SquareAttackedByColour(board, ksq2, InverseColour(colour)))) {
		// if all checks pass we can push kingside castle
		MoveListPush(movelist, MoveNew(source, ksq2) | QUEEN_CASTLE);
	    }
	}
    }
}

void PseudoCaptureKingGen(Board *board, MoveList *movelist) {
    Colour colour = board->turn;

    int source = LsbIndex(board->pieces[colour][King]);

    u64 dest_bb = KING_ATTACKS[source] & board->occupancy[InverseColour(colour)];

    // iterate through all the destinations
    while (dest_bb != 0) {
	int dest = LsbIndex(dest_bb);

	Move move = MoveNew(source,dest) | CAPTURE;

	MoveListPush(movelist, move);

	PopLsb(&dest_bb);
    }

    // we dont check for castle rights for king capture moves
}

// Pawns! yay yahoo!


