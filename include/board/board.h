#ifndef BOARD_H
#define BOARD_H
#include "globals.h"
#include "utils/types.h"
#include <stdio.h>

//
// Definitions for Board struct
//

#define WQC 0x1
#define WKC 0x2
#define BQC 0x4
#define BKC 0x8

// the main board structure
typedef struct {
    // array of all pieces. 0 = A1, 63 = H8
    Piece mailbox[64];

    // 0 white, 1 black | 0 pawn, 1 knight, 2 bishop, 3 rook, 4 queen, 5 king
    u64 pieces[2][6];

    // 0 white, 1 black, 2 all
    u64 occupancy[3];

    // current turn
    Colour turn;

    // castle rights. first 4 bits of u8. 0 WQC, 1 WKC, 2 BQC, 3 BKC
    u8 castle_rights;

    // en passant target square. the square number that can be captured, not the pawn.
    Square enpassant_tsq;

    // halfmove clock, useful for 50 move rule (100 halfmoves)
    u8 halfmove_clock;
} Board;

// creates a new board in startpos
Board BoardNew(void);

// prints board onto stdout
void BoardPrint(Board *self);

//
// Useful methods
//

// Temp function before incremental attack maps are implemented
bool SquareAttackedByColour(Board *board, Square sq, Colour colour);

bool InCheck(Board *board);

// MUST CLEAR BEFORE SETTING SQURE if you would like to place a piece there, because it doesnt overwrite
static inline void BoardSetSq(Board *self,Square sq,Piece piece) {
    Colour colour = PieceGetColourEnum(piece);
    self->mailbox[sq] = piece;
    self->pieces[colour][PieceGetTypeEnum(piece)] &= ~(1ULL << sq);
    self->occupancy[colour] |= (1ULL << sq);
    self->occupancy[2] |= (1ULL << sq);
}

// sets board square to empty if you know colour and piece type index
static inline void BoardClearSq(Board *self,Square sq,Piece piece) {
    Colour colour = PieceGetColourEnum(piece);
    self->mailbox[sq] = NULL_PIECE;
    self->pieces[colour][PieceGetTypeEnum(piece)] &= ~(1ULL << sq);
    self->occupancy[colour] &= ~(1ULL << sq);
    self->occupancy[2] &= ~(1ULL << sq);
}

#endif
