#include "eval/evaluation.h"
#include "utils/types.h"

// Piece values in centipawns
// Pawn,Knight,Bishop,Rook,Queen,King (king is 0 so it doesnt effect eval)
const int piece_values[6] = {100,300,350,500,900,0};

int StaticEvaluation(Board *board) {
    int eval = 0;
    int colour_constant;

    if (board->turn == White) {
	colour_constant = 1;
    } else {
	colour_constant = -1;
    }

    for (int i = 0; i < 64; i ++) {
	int value = piece_values[PieceGetType(board->mailbox[i])];
	int piece_colour_constant; 
	if (PieceGetColourEnum(board->mailbox[i]) == White) {
	    piece_colour_constant = 1;
	} else {
	    piece_colour_constant = -1;
	}

	eval += value * piece_colour_constant;
    }
    return eval * colour_constant;
}
