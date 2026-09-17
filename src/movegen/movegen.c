#include "movegen/movegen.h"
#include "board/board.h"
#include "movegen/magics.h"
#include "movegen/move.h"
#include "globals.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include <stdio.h>

void PseudoQuietKnightGen(const Board *board, MoveList *movelist) {
    u64 knights = board->pieces[board->turn][Knight];

    // iterate through all knights of current turn's colour
    while (knights) {
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

void PseudoCaptureKnightGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;
    u64 knights = board->pieces[colour][Knight];

    // iterate through all knights of current turn's colour
    while (knights) {
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

void PseudoQuietKingGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;

    int source = LsbIndex(board->pieces[colour][King]);

    // theres only gonna be 1 king so no need for while loop
    u64 dest_bb = KING_ATTACKS[source] & ~board->occupancy[ALL];

    // iterate through all the destinations
    while (dest_bb) {
	int dest = LsbIndex(dest_bb);

	Move move = MoveNew(source,dest);

	MoveListPush(movelist, move);

	PopLsb(&dest_bb);
    }

    // Castling 

    // since white = 0, black = 1, and castle rights in board are phyiscal bits in order WQC,WKC,BQC,BKC we can use this cool trick to shift to proper colour!
    int shift = 2*colour;

    // if we have castle kingside rights
    if (board->castle_rights & (WKC << shift)) {
	// check if squares are empty. because we know if we have castle rights kin will be in e4 or e8, shifting once and twice will give us the square king must pass through regardless of colour
	u64 ksq1 = (1ULL << (source+1)), ksq2 = (1ULL << (source+2));
	int ksq1i = LsbIndex(ksq1), ksq2i = LsbIndex(ksq2);
	if (!(board->occupancy[ALL] & ksq1) && !(board->occupancy[ALL] & ksq2)) {
	    // last check to see if ksq1 and ksq2 are currently under attack
	    if (!(SquareAttackedByColour(board, ksq1i, InverseColour(colour))) && !(SquareAttackedByColour(board, ksq2i, InverseColour(colour)))) {
		// if all checks pass we can push kingside castle
		if (!InCheck(board))
		    MoveListPush(movelist, MoveNew(source, ksq2i) | KING_CASTLE);
	    }
	}
    }

    // if we have castle queenside rights
    if (board->castle_rights & (WQC << shift)) {
	// check if squares are empty
	u64 ksq1 = (1ULL << (source-1)), ksq2 = (1ULL << (source-2)),ksq3 = (1ULL << (source-3));
	int ksq1i = LsbIndex(ksq1), ksq2i = LsbIndex(ksq2);
	if (!(board->occupancy[ALL] & ksq1) && !(board->occupancy[ALL] & ksq2) && !(board->occupancy[ALL] & ksq3)) {
	    // last check to see if ksq1 and ksq2 are currently under attack
	    if (!(SquareAttackedByColour(board, ksq1i, InverseColour(colour))) && !(SquareAttackedByColour(board, ksq2i, InverseColour(colour)))) {
		// if all checks pass we can push kingside castle
		if (!InCheck(board))
		    MoveListPush(movelist, MoveNew(source, ksq2i) | QUEEN_CASTLE);
	    }
	}
    }
}

void PseudoCaptureKingGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;

    int source = LsbIndex(board->pieces[colour][King]);

    u64 dest_bb = KING_ATTACKS[source] & board->occupancy[InverseColour(colour)];

    // iterate through all the destinations
    while (dest_bb) {
	int dest = LsbIndex(dest_bb);

	Move move = MoveNew(source,dest) | CAPTURE;

	MoveListPush(movelist, move);

	PopLsb(&dest_bb);
    }

    // we dont check castling for king capture moves
}

void PseudoQuietPawnGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;

    u64 pawns = board->pieces[colour][Pawn];
    u64 free = ~board->occupancy[ALL];

    // if white
    if (colour == White) {
	// make attack bitboards
	u64 single_push = (pawns << 8) & free;
	u64 double_push = ((single_push & RANK_3) << 8) & free;
	u64 promotion = (single_push & RANK_8);
	single_push &= ~RANK_8;

	// push moves
	while (single_push) {
	    int dest = LsbIndex(single_push);

	    MoveListPush(movelist, MoveNew(dest - 8, dest) | SINGLE_PAWN_PUSH);

	    PopLsb(&single_push);
	}

	while (double_push) {
	    int dest = LsbIndex(double_push);

	    MoveListPush(movelist, MoveNew(dest - 16, dest) | DOUBLE_PAWN_PUSH);

	    PopLsb(&double_push);
	}

	while (promotion) {
	    int dest = LsbIndex(promotion);

	    MoveListPush(movelist, MoveNew(dest - 8, dest) | KNIGHT_PROMO);
	    MoveListPush(movelist, MoveNew(dest - 8, dest) | BISHOP_PROMO);
	    MoveListPush(movelist, MoveNew(dest - 8, dest) | ROOK_PROMO);
	    MoveListPush(movelist, MoveNew(dest - 8, dest) | QUEEN_PROMO);

	    PopLsb(&promotion);
	}

    // if black
    } else {
	// make attack bitboards
	u64 single_push = (pawns >> 8) & free;
	u64 double_push = ((single_push & RANK_6) >> 8) & free;
	u64 promotion = (single_push & RANK_1);
	single_push &= ~RANK_1;

	// push moves
	while (single_push) {
	    int dest = LsbIndex(single_push);

	    MoveListPush(movelist, MoveNew(dest + 8, dest) | SINGLE_PAWN_PUSH);

	    PopLsb(&single_push);
	}

	while (double_push) {
	    int dest = LsbIndex(double_push);

	    MoveListPush(movelist, MoveNew(dest + 16, dest) | DOUBLE_PAWN_PUSH);

	    PopLsb(&double_push);
	}

	while (promotion) {
	    int dest = LsbIndex(promotion);

	    MoveListPush(movelist, MoveNew(dest + 8, dest) | KNIGHT_PROMO);
	    MoveListPush(movelist, MoveNew(dest + 8, dest) | BISHOP_PROMO);
	    MoveListPush(movelist, MoveNew(dest + 8, dest) | ROOK_PROMO);
	    MoveListPush(movelist, MoveNew(dest + 8, dest) | QUEEN_PROMO);

	    PopLsb(&promotion);
	}
    }
}

void PseudoCapturePawnGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;

    u64 pawns = board->pieces[colour][Pawn];
    u64 enemy_occ = board->occupancy[InverseColour(colour)];

    // if white
    if (colour == White) {
	u64 left_captures = ((pawns << 7) & ~FILE_H);
	u64 right_captures = ((pawns << 9) & ~FILE_A);

	// if en passant sqaure exists check if we can capture
	if (board->enpassant_tsq != NullSquare) {
	    u64 enpassant_tsq_bb = (1ULL << board->enpassant_tsq);

	    int left_enpassant = LsbIndex(left_captures & enpassant_tsq_bb);
	    int right_enpassant = LsbIndex(right_captures & enpassant_tsq_bb);

	    if (left_enpassant != NullSquare) {
		MoveListPush(movelist, MoveNew(left_enpassant - 7, left_enpassant) | EP_CAPTURE);
	    }

	    if (right_enpassant != NullSquare) {
		MoveListPush(movelist, MoveNew(right_enpassant - 9, right_enpassant) | EP_CAPTURE);
	    }
	}

	left_captures &= enemy_occ;
	right_captures &= enemy_occ;

	// promotions
	u64 left_capture_promotions = left_captures & RANK_8;
	u64 right_capture_promotions = right_captures & RANK_8;

	left_captures &= ~RANK_8;
	right_captures &= ~RANK_8;

	// push moves

	while (left_capture_promotions) {
	    int dest = LsbIndex(left_capture_promotions);

	    MoveListPush(movelist, MoveNew(dest - 7, dest) | KNIGHT_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest - 7, dest) | BISHOP_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest - 7, dest) | ROOK_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest - 7, dest) | QUEEN_PROMO_CAPTURE);

	    PopLsb(&left_capture_promotions);
	}
	
	while (right_capture_promotions) {
	    int dest = LsbIndex(right_capture_promotions);

	    MoveListPush(movelist, MoveNew(dest - 9, dest) | KNIGHT_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest - 9, dest) | BISHOP_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest - 9, dest) | ROOK_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest - 9, dest) | QUEEN_PROMO_CAPTURE);

	    PopLsb(&right_capture_promotions);
	}

	while (left_captures) {
	    int dest = LsbIndex(left_captures);

	    MoveListPush(movelist, MoveNew(dest - 7, dest) | CAPTURE);

	    PopLsb(&left_captures);
	}

	while (right_captures) {
	    int dest = LsbIndex(right_captures);

	    MoveListPush(movelist, MoveNew(dest - 9, dest) | CAPTURE);

	    PopLsb(&right_captures);
	}
    // if black
    } else {
	u64 left_captures = ((pawns >> 9) & ~FILE_H);
	u64 right_captures = ((pawns >> 7) & ~FILE_A);

	// if en passant sqaure exists check if we can capture
	if (board->enpassant_tsq != NullSquare) {
	    u64 enpassant_tsq_bb = (1ULL << board->enpassant_tsq);

	    int left_enpassant = LsbIndex(left_captures & enpassant_tsq_bb);
	    int right_enpassant = LsbIndex(right_captures & enpassant_tsq_bb);

	    if (left_enpassant != NullSquare) {
		MoveListPush(movelist, MoveNew(left_enpassant + 9, left_enpassant) | EP_CAPTURE);
	    }

	    if (right_enpassant != NullSquare) {
		MoveListPush(movelist, MoveNew(right_enpassant + 7, right_enpassant) | EP_CAPTURE);
	    }
	}

	left_captures &= enemy_occ;
	right_captures &= enemy_occ;

	// promotions
	u64 left_capture_promotions = left_captures & RANK_1;
	u64 right_capture_promotions = right_captures & RANK_1;

	left_captures &= ~RANK_1;
	right_captures &= ~RANK_1;

	// push moves

	while (left_capture_promotions) {
	    int dest = LsbIndex(left_capture_promotions);

	    MoveListPush(movelist, MoveNew(dest + 9, dest) | KNIGHT_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest + 9, dest) | BISHOP_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest + 9, dest) | ROOK_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest + 9, dest) | QUEEN_PROMO_CAPTURE);

	    PopLsb(&left_capture_promotions);
	}
	
	while (right_capture_promotions) {
	    int dest = LsbIndex(right_capture_promotions);

	    MoveListPush(movelist, MoveNew(dest + 7, dest) | KNIGHT_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest + 7, dest) | BISHOP_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest + 7, dest) | ROOK_PROMO_CAPTURE);
	    MoveListPush(movelist, MoveNew(dest + 7, dest) | QUEEN_PROMO_CAPTURE);

	    PopLsb(&right_capture_promotions);
	}

	while (left_captures) {
	    int dest = LsbIndex(left_captures);

	    MoveListPush(movelist, MoveNew(dest + 9, dest) | CAPTURE);

	    PopLsb(&left_captures);
	}

	while (right_captures) {
	    int dest = LsbIndex(right_captures);

	    MoveListPush(movelist, MoveNew(dest + 7, dest) | CAPTURE);

	    PopLsb(&right_captures);
	}

    }
}

// sliders

void PseudoQuietRookGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;
    u64 rooks = board->pieces[colour][Rook];
    u64 occ = board->occupancy[ALL];

    // iter through all rooks
    while (rooks) {
	Square source = LsbIndex(rooks);

	// get all rook quiets
	u64 attacks = RookAttackFromSquare(source, occ) & ~occ;

	while (attacks) {
	    Square dest = LsbIndex(attacks);

	    // push move
	    MoveListPush(movelist, MoveNew(source, dest));
	    
	    PopLsb(&attacks);
	}

	PopLsb(&rooks);
    }
}

void PseudoCaptureRookGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;
    u64 rooks = board->pieces[colour][Rook];

    // iter through all rooks
    while (rooks) {
	Square source = LsbIndex(rooks);

	// get all rook captures
	u64 attacks = RookAttackFromSquare(source, board->occupancy[ALL]) & board->occupancy[InverseColour(colour)];

	while (attacks) {
	    Square dest = LsbIndex(attacks);

	    // push move
	    MoveListPush(movelist, MoveNew(source, dest) | CAPTURE);
	    
	    PopLsb(&attacks);
	}

	PopLsb(&rooks);
    }
}

void PseudoQuietBishopGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;
    u64 bishops = board->pieces[colour][Bishop];
    u64 occ = board->occupancy[ALL];

    // iter through all bishops
    while (bishops) {
	Square source = LsbIndex(bishops);

	// get all bishop quiets
	u64 attacks = BishopAttackFromSquare(source, occ) & ~occ;

	while (attacks) {
	    Square dest = LsbIndex(attacks);

	    // push move
	    MoveListPush(movelist, MoveNew(source, dest));
	    
	    PopLsb(&attacks);
	}

	PopLsb(&bishops);
    }
}

void PseudoCaptureBishopGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;
    u64 bishops = board->pieces[colour][Bishop];

    // iter through all bishops
    while (bishops) {
	Square source = LsbIndex(bishops);

	// get all bishop captures
	u64 attacks = BishopAttackFromSquare(source, board->occupancy[ALL]) & board->occupancy[InverseColour(colour)];

	while (attacks) {
	    Square dest = LsbIndex(attacks);

	    // push move
	    MoveListPush(movelist, MoveNew(source, dest) | CAPTURE);
	    
	    PopLsb(&attacks);
	}

	PopLsb(&bishops);
    }
}

void PseudoQuietQueenGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;
    u64 queens = board->pieces[colour][Queen];
    u64 occ = board->occupancy[ALL];

    // iter through all queens
    while (queens) {
	Square source = LsbIndex(queens);

	// get all queen quiets
	u64 attacks = (BishopAttackFromSquare(source, occ) | RookAttackFromSquare(source, occ)) & ~occ;

	while (attacks) {
	    Square dest = LsbIndex(attacks);

	    // push move
	    MoveListPush(movelist, MoveNew(source, dest));
	    
	    PopLsb(&attacks);
	}

	PopLsb(&queens);
    }
}

void PseudoCaptureQueenGen(const Board *board, MoveList *movelist) {
    Colour colour = board->turn;
    u64 queens = board->pieces[colour][Queen];
    u64 occ = board->occupancy[ALL];

    // iter through all queens
    while (queens) {
	Square source = LsbIndex(queens);

	// get all queen captures
	u64 attacks = (BishopAttackFromSquare(source, occ) | RookAttackFromSquare(source, occ)) & board->occupancy[InverseColour(colour)];

	while (attacks) {
	    Square dest = LsbIndex(attacks);

	    // push move
	    MoveListPush(movelist, MoveNew(source, dest) | CAPTURE);
	    
	    PopLsb(&attacks);
	}

	PopLsb(&queens);
    }
}

void GenPseudoLegalMoves(const Board *board, MoveList *movelist) {
    GenPseudoLegalCaptures(board, movelist);
    GenPseudoLegalQuiets(board, movelist);
}

void GenPseudoLegalCaptures(const Board *board, MoveList *movelist) {
    PseudoCapturePawnGen(board, movelist);
    PseudoCaptureKnightGen(board, movelist);
    PseudoCaptureBishopGen(board, movelist);
    PseudoCaptureRookGen(board, movelist);
    PseudoCaptureQueenGen(board, movelist);
    PseudoCaptureKingGen(board, movelist);
}

void GenPseudoLegalQuiets(const Board *board, MoveList *movelist) {
    PseudoQuietPawnGen(board, movelist);
    PseudoQuietKnightGen(board, movelist);
    PseudoQuietBishopGen(board, movelist);
    PseudoQuietRookGen(board, movelist);
    PseudoQuietQueenGen(board, movelist);
    PseudoQuietKingGen(board, movelist);
}

