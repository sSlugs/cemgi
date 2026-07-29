#include "utils/types.h"
#include <ctype.h>

char PieceToChar(Piece piece) {
    int colour = PieceGetColourEnum(piece);

    char c = ' ';

    if (piece == NULL_PIECE) {
	return c;
    }

    switch (PieceGetTypeEnum(piece)) {
	case Pawn:
	    c = 'p';
	    break;

	case Knight:
	    c = 'n';
	    break;

	case Bishop:
	    c = 'b';
	    break;

	case Rook:
	    c = 'r';
	    break;

	case Queen:
	    c = 'q';
	    break;

	case King:
	    c = 'k';
	    break;
    }

    if (colour == White) {
	return toupper(c);
    }

    return c;
}
