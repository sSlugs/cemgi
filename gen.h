#ifndef GEN_H
#define GEN_H
#include <stdint.h>

// typedefs for unsigned ints
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

// FILES
#define FILE_A 0x101010101010101
#define FILE_B 0x202020202020202
#define FILE_C 0x404040404040404
#define FILE_D 0x808080808080808
#define FILE_E 0x1010101010101010
#define FILE_F 0x2020202020202020
#define FILE_G 0x4040404040404040
#define FILE_H 0x8080808080808080

void KnightAttacks(void);
void KingAttacks(void);
void SquareMasks(void);
void InverseSquareMasks(void);

#endif
