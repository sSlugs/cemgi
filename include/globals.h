#ifndef GLOBALS_H
#define GLOBALS_H
 
#include <stdint.h>
#include <inttypes.h>
#include <inttypes.h>

// for occupancy, helps readability
#define ALL 2

// typedefs for unsigned ints
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

// functions

// turns string to a int
int StringToNumber(char *str);
int ToNum(const char c);

// generates a new random u64 (Strong, uses linux API)
u64 RandomU64(void);

// FILES
#define FILE_A 0x101010101010101ULL
#define FILE_B 0x202020202020202ULL
#define FILE_C 0x404040404040404ULL
#define FILE_D 0x808080808080808ULL
#define FILE_E 0x1010101010101010ULL
#define FILE_F 0x2020202020202020ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

extern const u64 FILES[8];

// RANKS
#define RANK_1 0xFFULL 
#define RANK_2 0xFF00ULL 
#define RANK_3 0xFF0000ULL
#define RANK_4 0xFF000000ULL
#define RANK_5 0xFF00000000ULL
#define RANK_6 0xFF0000000000ULL
#define RANK_7 0xFF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

extern const u64 RANKS[8];

// ATTACK TABLES
extern const u64 PAWN_ATTACKS[2][64];
extern const u64 KNIGHT_ATTACKS[64];
extern const u64 KING_ATTACKS[64];

#endif
