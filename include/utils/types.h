#ifndef TYPES_H
#define TYPES_H
#include <globals.h>
#include <stdbool.h>

//
// Colour type functions and definitions
//

typedef enum {
    White,
    Black,
} Colour;

static inline Colour InverseColour(Colour colour) {
    return colour ^ 1;
}

//
// Piece type functions and definitions
//

// useful replacements (OR together to make full pieces)
#define PAWN 0x0 
#define KNIGHT 0x1 
#define BISHOP 0x2 
#define ROOK 0x3 
#define QUEEN 0x4 
#define KING 0x5 

#define WHITE_PIECE 0x0
#define BLACK_PIECE 0x8

#define NULL_PIECE 0xff

typedef uint8_t Piece;

// Use enums for indexing stuff

typedef enum {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
} PieceType;

// gets piece type. ex. pawn, knight, etc
static inline Piece PieceGetType(Piece piece) {
    return piece & 0x7;
}

// gets piece type. ex. pawn, knight, etc as an enum
static inline PieceType PieceGetTypeEnum(Piece piece) {
    return (PieceType)(piece & 0x7);
}

// gets piece colour as a u8
static inline u8 PieceGetColourU8(Piece piece) {
    return piece & 0x8;
}

// gets piece colour as an enum
static inline Colour PieceGetColourEnum(Piece piece) {
    int colour = piece & 0x8;
    if (colour == 8) {
	return Black;
    } else {
	return White;
    }
}

static inline bool PieceIsNull(Piece piece) {
    return piece == NULL_PIECE;
}

char PieceToChar(Piece piece);

//
// Square type functions and definitions
//

typedef enum {
    A1,B1,C1,D1,E1,F1,G1,H1,
    A2,B2,C2,D2,E2,F2,G2,H2,
    A3,B3,C3,D3,E3,F3,G3,H3,
    A4,B4,C4,D4,E4,F4,G4,H4,
    A5,B5,C5,D5,E5,F5,G5,H5,
    A6,B6,C6,D6,E6,F6,G6,H6,
    A7,B7,C7,D7,E7,F7,G7,H7,
    A8,B8,C8,D8,E8,F8,G8,H8,
    NullSquare,
} Square;

#endif
