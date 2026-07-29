#include "board/board.h"
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

bool SquareAttackedByColour(Board *self, Square sq, Colour colour) {

    return false;
}
