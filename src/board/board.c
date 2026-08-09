#include "board/board.h"
#include "globals.h"
#include "movegen/magics.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include <stdio.h>

//
// Code for Board struct
//

Board BoardNew(void) {
Board newboard = {
	.pieces = {
	    {0xff00,0x42,0x24,0x81,0x8,0x10},
	    {0xff000000000000,0x4200000000000000,0x2400000000000000,0x8100000000000000,0x800000000000000,0x1000000000000000}
	},
	
	.occupancy = {
	    0xffff,
	    0xffff000000000000,
	    0xffff00000000ffff
	},

	.mailbox = {
	    ROOK,KNIGHT,BISHOP,QUEEN,KING,BISHOP,KNIGHT,ROOK,
	    PAWN,PAWN,PAWN,PAWN,PAWN,PAWN,PAWN,PAWN,
	    NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,
	    NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,
	    NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,
	    NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,NULL_PIECE,
	    BLACK_PIECE | PAWN,BLACK_PIECE | PAWN,BLACK_PIECE | PAWN,BLACK_PIECE | PAWN,BLACK_PIECE | PAWN,BLACK_PIECE | PAWN,BLACK_PIECE | PAWN,BLACK_PIECE | PAWN,
	    BLACK_PIECE | ROOK,BLACK_PIECE | KNIGHT,BLACK_PIECE | BISHOP,BLACK_PIECE | QUEEN,BLACK_PIECE | KING,BLACK_PIECE | BISHOP,BLACK_PIECE | KNIGHT,BLACK_PIECE | ROOK,
	},

	.turn = White,

	.castle_rights = 0xf,

	.enpassant_tsq = NullSquare,

	.halfmove_clock = 0
    };

    return newboard;
}

Board BoardFEN(char *fen) {
    Board board;

    
    
    return board;
}

void BoardPrint(Board *self) {
    printf("\n +---+---+---+---+---+---+---+---+ \n");
    // rows
    for (int r = 7; r >= 0; r--) {
	// cols
	printf(" |");
	for (int c = 0; c < 8; c++) {
	    char ch = PieceToChar(self->mailbox[(8*r + c)]);
	    printf(" %c |", ch);
	}
	printf(" %d ",r+1);
	printf("\n +---+---+---+---+---+---+---+---+ \n");
    }
    printf("   A   B   C   D   E   F   G   H\n\n");
}

bool SquareAttackedByColour(Board *board, Square sq, Colour colour) {
    // shoot rays from square and look for collisions
    u64 occ = board->occupancy[ALL];

    // knight
    if (KNIGHT_ATTACKS[sq] & board->pieces[colour][Knight])
	return true;

    // bishop / queen
    u64 bb = BishopAttackFromSquare(sq, occ);
    if ((bb & board->pieces[colour][Bishop]) || (bb & board->pieces[colour][Queen]))
	return true;

    // rook / queen
    bb = RookAttackFromSquare(sq, occ);
    if ((bb & board->pieces[colour][Rook]) || (bb & board->pieces[colour][Queen]))
	return true;

    // pawn
    if (PAWN_ATTACKS[InverseColour(colour)][sq] & board->pieces[colour][Pawn])
	return true;
    
    // king
    if (KING_ATTACKS[sq] & board->pieces[colour][King])
	return true;

    return false;
}

bool InCheck(Board *board) {
    Colour turn = board->turn;
    Square king_sq = LsbIndex(board->pieces[turn][King]);

    if (SquareAttackedByColour(board, king_sq, InverseColour(turn)))
	return true;

    return false;
}

bool EnemyInCheck(Board *board) {
    Colour turn = board->turn;
    Square king_sq = LsbIndex(board->pieces[InverseColour(turn)][King]);

    if (SquareAttackedByColour(board, king_sq, turn))
	return true;

    return false;
}
