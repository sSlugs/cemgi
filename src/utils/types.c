#include "utils/types.h"
#include <ctype.h>

static int GetCol(const char c) {
    switch (c) {
	case 'a':
	    return 0;
	case 'b':
	    return 1;
	case 'c':
	    return 2;
	case 'd':
	    return 3;
	case 'e':
	    return 4;
	case 'f':
	    return 5;
	case 'g':
	    return 6;
	case 'h':
	    return 7;
    }
    return 0;
}

static int GetRowStart(const char c) {
    switch (c) {
	case '1':
	    return 0;
	case '2':
	    return 8;
	case '3':
	    return 16;
	case '4':
	    return 24;
	case '5':
	    return 32;
	case '6':
	    return 40;
	case '7':
	    return 48;
	case '8':
	    return 56;
    }
    return 0;
}

Square StrToSquare(char *str) {
    return GetRowStart(str[0]) + GetCol(str[1]);
}

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
