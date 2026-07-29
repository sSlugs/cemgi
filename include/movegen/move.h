#ifndef MOVE_H
#define MOVE_H
#include "utils/types.h"
#include <stddef.h>
#include <stdint.h>

/*
    Encoding: first 6 bits are the SOURCE sq (6), the next 6 bits are DEST square (12),
    13th bit is promotion, 14th bit is capture, 15th is special 1. and 16th is special 2.
    mix of the specials determine move type, see table below

    code  |  promotion   capture    special 1    special 0  | Move
     0    |      0          0           0            0      |  quiet move
     1    |      0          0           0            1      |  double pawn push
     2    |      0          0           1            0      |  king castle
     3    |      0          0           1            1      |  queen castle
     4    |      0          1           0            0      |  capture
     5    |      0          1           0            1      |  EP capture
     6    |      0          1           1            0      |  (UNUSED)
     7    |      0          1           1            1      |  (UNUSED)
     8    |      1          0           0            0      |  knight-promo
     9    |      1          0           0            1      |  bishop-promo
     10   |      1          0           1            0      |  rook-promo
     11   |      1          0           1            1      |  queen-promo
     12   |      1          1           0            0      |  knight-promo captured
     13   |      1          1           0            1      |  bishop-promo capture
     14   |      1          1           1            0      |  rook-promo capture
     15   |      1          1           1            1      |  queen-promo capture
*/

// macros!!!!!
#define QUIET_MOVE 0x0
#define DOUBLE_PAWN_PUSH 0x1000
#define KING_CASTLE 0x2000
#define QUEEN_CASTLE 0x3000
#define CAPTURE 0x4000
#define EP_CAPTURE 0x5000
// skip 0x6000, and 0x7000 as they arent being used
#define KNIGHT_PROMO 0x8000
#define BISHOP_PROMO 0x9000
#define ROOK_PROMO 0xa000
#define QUEEN_PROMO 0xb000
#define KNIGHT_PROMO_CAPTURE 0xc000
#define BISHOP_PROMO_CAPTURE 0xd000
#define ROOK_PROMO_CAPTURE 0xa000
#define QUEEN_PROMO_CAPTURE 0xf000

//
// Move stuff
//

typedef uint16_t Move;

// must bitwise OR the flag onto the move after
static inline Move MoveNew(Square src, Square dst) {
    return (Move)(src | (dst << 6));
}

//
// Movelist stuff
//

// movelist structure. holds moves, crazy.
typedef struct {
    // allocated 256 Moves always
    Move data[256];
    // current number of moves, also can be used as a pointer to the start of next free space
    size_t len;
} MoveList;

static inline void MoveListPush(MoveList *self, Move move) {
    self->data[self->len++] = move;
}

MoveList MoveListNew(void);

#endif
