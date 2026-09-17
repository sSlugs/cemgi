#include "board/board.h"
#include "globals.h"
#include "movegen/magics.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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


static PieceType GetPieceType(const char c) {
    char lowerc = tolower(c);
    switch (lowerc) {
	case 'p':
	    return Pawn;
	case 'n':
	    return Knight;
	case 'b':
	    return Bishop;
	case 'r':
	    return Rook;
	case 'q':
	    return Queen;
    }

    return King;
}

static u8 GetCastleRight(const char c) {
    switch (c) {
	case 'K':
	    return WKC;
	case 'Q':
	    return WQC;
	case 'k':
	    return BKC;
	case 'q':
	    return BQC;
    }
    return 0;
}

Board BoardFEN(const char *fen_s) {
    Board board = {0};
    char fen[1024];
    memcpy(fen, fen_s,strlen(fen_s));

    // iter through all fields
    int field_num = 0;
    char *field = strtok(fen, " ");
    int char_pointer = 0;
    while (field != NULL) {
	int pointer = 0;
	switch (field_num) {
	    // Board position
	    case 0:
		// for each row from 7-1;
		for (int i = 7 ; i >= 0; i--) {
		    // set board to what fen says by looping through
		    for (int j = 0; j < 9; j++) {
			char piece = field[char_pointer++];
			if (piece == '\0')
			    break;
			int num = ToNum(piece);

			// if we finished row
			if (piece == '/') {
			    pointer = 0;
			    break; 
			} else if (!num) {
			    // what square the piece should be at for board
			    int sq_pointer = 8*i + pointer++;
			    Colour colour = White;
			    PieceType type = GetPieceType(piece);
			    if islower(piece)
				colour = Black;
			    SetBit(&board.pieces[colour][type], sq_pointer);
			    SetBit(&board.occupancy[colour], sq_pointer);
			    SetBit(&board.occupancy[ALL], sq_pointer);
			    board.mailbox[sq_pointer] = (Piece)(type | (BLACK_PIECE * colour));
			} else {
			    for (int k = 0; k < num; k++) {
				int sq_pointer = 8*i + pointer++;
				board.mailbox[sq_pointer] = NULL_PIECE;
			    }
			}
		    }
		}
		break;

	    // turn
	    case 1:
		if (field[0] == 'w')
		    board.turn = White;
		else
		    board.turn = Black;
		break;

	    // castling
	    case 2:
		while(1) {
		    char cr = GetCastleRight(field[pointer++]);
		    if (!cr)
			break;
		    board.castle_rights |= cr;
		}
		break;
	    
	    // en passant
	    case 3:
		if (field[0] == '-')
		    board.enpassant_tsq = NullSquare;
		else {
		    board.enpassant_tsq = StrToSquare(field);
		}
		break;

	    //
	    case 4:
		board.halfmove_clock = StringToNumber(field);
	}
	
	field = strtok(NULL, " ");
	field_num++;
    }
    
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
