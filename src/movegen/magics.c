#include "movegen/magics.h"
#include "globals.h"
#include "utils/types.h"
#include "utils/bitboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//
// Precomputed tables
//

// number of relevant bits on each mask at each square
const int ROOK_RELEVANT_BITS[64] = {
0xc, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xc, 
0xb, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xb, 
0xb, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xb, 
0xb, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xb, 
0xb, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xb, 
0xb, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xb, 
0xb, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xb, 
0xc, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xc, 
};
const int BISHOP_RELEVANT_BITS[64] = {
0x6, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x6, 
0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 
0x5, 0x5, 0x7, 0x7, 0x7, 0x7, 0x5, 0x5, 
0x5, 0x5, 0x7, 0x9, 0x9, 0x7, 0x5, 0x5, 
0x5, 0x5, 0x7, 0x9, 0x9, 0x7, 0x5, 0x5, 
0x5, 0x5, 0x7, 0x7, 0x7, 0x7, 0x5, 0x5, 
0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 
0x6, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x6, 
};

// relevant bit masks, the actual squares that matter for each square
const u64 ROOK_MASKS[64] = {
0x101010101017eULL, 0x202020202027cULL, 0x404040404047aULL, 0x8080808080876ULL, 
0x1010101010106eULL, 0x2020202020205eULL, 0x4040404040403eULL, 0x8080808080807eULL, 
0x1010101017e00ULL, 0x2020202027c00ULL, 0x4040404047a00ULL, 0x8080808087600ULL, 
0x10101010106e00ULL, 0x20202020205e00ULL, 0x40404040403e00ULL, 0x80808080807e00ULL, 
0x10101017e0100ULL, 0x20202027c0200ULL, 0x40404047a0400ULL, 0x8080808760800ULL, 
0x101010106e1000ULL, 0x202020205e2000ULL, 0x404040403e4000ULL, 0x808080807e8000ULL, 
0x101017e010100ULL, 0x202027c020200ULL, 0x404047a040400ULL, 0x8080876080800ULL, 
0x1010106e101000ULL, 0x2020205e202000ULL, 0x4040403e404000ULL, 0x8080807e808000ULL, 
0x1017e01010100ULL, 0x2027c02020200ULL, 0x4047a04040400ULL, 0x8087608080800ULL, 
0x10106e10101000ULL, 0x20205e20202000ULL, 0x40403e40404000ULL, 0x80807e80808000ULL, 
0x17e0101010100ULL, 0x27c0202020200ULL, 0x47a0404040400ULL, 0x8760808080800ULL, 
0x106e1010101000ULL, 0x205e2020202000ULL, 0x403e4040404000ULL, 0x807e8080808000ULL, 
0x7e010101010100ULL, 0x7c020202020200ULL, 0x7a040404040400ULL, 0x76080808080800ULL, 
0x6e101010101000ULL, 0x5e202020202000ULL, 0x3e404040404000ULL, 0x7e808080808000ULL, 
0x7e01010101010100ULL, 0x7c02020202020200ULL, 0x7a04040404040400ULL, 0x7608080808080800ULL, 
0x6e10101010101000ULL, 0x5e20202020202000ULL, 0x3e40404040404000ULL, 0x7e80808080808000ULL, 
};
const u64 BISHOP_MASKS[64] = {
0x40201008040200ULL, 0x402010080400ULL, 0x4020100a00ULL, 0x40221400ULL, 
0x2442800ULL, 0x204085000ULL, 0x20408102000ULL, 0x2040810204000ULL, 
0x20100804020000ULL, 0x40201008040000ULL, 0x4020100a0000ULL, 0x4022140000ULL, 
0x244280000ULL, 0x20408500000ULL, 0x2040810200000ULL, 0x4081020400000ULL, 
0x10080402000200ULL, 0x20100804000400ULL, 0x4020100a000a00ULL, 0x402214001400ULL, 
0x24428002800ULL, 0x2040850005000ULL, 0x4081020002000ULL, 0x8102040004000ULL, 
0x8040200020400ULL, 0x10080400040800ULL, 0x20100a000a1000ULL, 0x40221400142200ULL, 
0x2442800284400ULL, 0x4085000500800ULL, 0x8102000201000ULL, 0x10204000402000ULL, 
0x4020002040800ULL, 0x8040004081000ULL, 0x100a000a102000ULL, 0x22140014224000ULL, 
0x44280028440200ULL, 0x8500050080400ULL, 0x10200020100800ULL, 0x20400040201000ULL, 
0x2000204081000ULL, 0x4000408102000ULL, 0xa000a10204000ULL, 0x14001422400000ULL, 
0x28002844020000ULL, 0x50005008040200ULL, 0x20002010080400ULL, 0x40004020100800ULL, 
0x20408102000ULL, 0x40810204000ULL, 0xa1020400000ULL, 0x142240000000ULL, 
0x284402000000ULL, 0x500804020000ULL, 0x201008040200ULL, 0x402010080400ULL, 
0x2040810204000ULL, 0x4081020400000ULL, 0xa102040000000ULL, 0x14224000000000ULL, 
0x28440200000000ULL, 0x50080402000000ULL, 0x20100804020000ULL, 0x40201008040200ULL, 
};

// the magic numbers for hash 
const u64 ROOK_MAGICS[64] = {
0x1600008241201200ULL, 0x14800050a002c008ULL, 0x4600008020120440ULL, 0xb080008284081000ULL, 
0x8000280824000aULL, 0x5000008509902010ULL, 0x8800000430080660ULL, 0x812000020080204ULL, 
0x90000800020c0ULL, 0x8008000ae80c406ULL, 0x100800c20008010ULL, 0x8043000020300102ULL, 
0x8050400008001004ULL, 0x2800420410080bULL, 0x204400008200900ULL, 0x42840000808c098ULL, 
0xc500020002010020ULL, 0x40a08000824008ULL, 0x9800024000844031ULL, 0x2044004000400800ULL, 
0xc180010000486004ULL, 0x410008002008c00ULL, 0x800402000801300ULL, 0x40080000120c4ULL, 
0x882000600008044ULL, 0x2001210080005084ULL, 0x8000010026048ULL, 0x70004040000908ULL, 
0x60040080000802ULL, 0x118a3100000a020ULL, 0x410042030000280cULL, 0x1008044028000680ULL, 
0x8a0601000a000020ULL, 0x400550022001210ULL, 0x22c801000806000ULL, 0xc210402000a00400ULL, 
0xa080402800080ULL, 0x4040080204000020ULL, 0x400150000400094ULL, 0xc001128044000008ULL, 
0x1000228000410000ULL, 0xa84000002004ULL, 0x4810200880000600ULL, 0x4020221000002000ULL, 
0x2800400028000ULL, 0x1450a4821040000ULL, 0x6040220004010000ULL, 0x5044200040002000ULL, 
0x108002a8c0000080ULL, 0x8808108400800610ULL, 0x4220100020000104ULL, 0x1500980010000088ULL, 
0xd008000804000080ULL, 0x10080682401000ULL, 0x104020000a0ULL, 0x40090122408400ULL, 
0x800418020170001ULL, 0x204002318005ULL, 0x2020000a144001ULL, 0x1010021005002001ULL, 
0x19000426180001ULL, 0x8004024054681090ULL, 0x89410203108ULL, 0x880210380882050cULL, 
};
const u64 BISHOP_MAGICS[64] = {
0x2000281000802106ULL, 0x48ee4090104ULL, 0x18110403002ULL, 0x104100114010ULL, 
0x1680a04000100e0ULL, 0xd00020242020020ULL, 0x2000c0224c44002ULL, 0x8080010301014842ULL, 
0x104002028018080ULL, 0x14000902608200ULL, 0x2000254820120ULL, 0x1c00002400203918ULL, 
0x8800000840400080ULL, 0x2400008a8140104ULL, 0x402401243000ULL, 0x380100501282000ULL, 
0x68004080880ULL, 0x2100a41002018400ULL, 0x6090008ULL, 0x18104002044200ULL, 
0x5600000401a00050ULL, 0x8000042002011000ULL, 0x108008180200ULL, 0x1820080404108c80ULL, 
0x21400280102b000ULL, 0x1000000110480ULL, 0x102800180004010ULL, 0x4000340010004ULL, 
0x409000400004000ULL, 0x80200008200809aULL, 0x8602100000089820ULL, 0x41100004800980eULL, 
0x2208040408002402ULL, 0x24a8040ba00404ULL, 0x100410040800400ULL, 0x40821000000081ULL, 
0x100c20001694044ULL, 0x1420251000400480ULL, 0x208100480c220ULL, 0x4801010000800040ULL, 
0x802300288000004ULL, 0x4440101100801ULL, 0x4101004200004007ULL, 0x20059101000040ULL, 
0x31088200000800ULL, 0x85401000800c4000ULL, 0x36502240000008ULL, 0x8020040000000ULL, 
0x8300820802000248ULL, 0x18080cf10011000ULL, 0x1080084049008000ULL, 0x2800001a20882051ULL, 
0x801110400010ULL, 0xa0d00685000852ULL, 0x422020418028060ULL, 0x2104202000021ULL, 
0x29840115920000ULL, 0x20082010080ULL, 0x30a008a0020ULL, 0xd0100440008ULL, 
0x80200092134800cULL, 0x220908c010012000ULL, 0x200100208080020ULL, 0x1484200c20408000ULL, 
};

// the actual attacks that can be indexed by the the magic index
u64 ROOK_ATTACKS[64][4096] = {};
u64 BISHOP_ATTACKS[64][512] = {};

//
// Move generation on the fly
//

// i could use hyperbola quintessence but ion care the actualy speed of engine is the same and it shouldnt matter. we just loop through

static u64 RookAttacksOnFly(Square square,u64 occupancy) {
    u64 attacks = 0;
    u64 base = 1ULL << square;
    u64 rank_occ = RANKS[square / 8];
    u64 empty = ~occupancy;

    u64 attack;

    // north ray
    attack = base;
    do {
	attack <<= 8;
	attacks |= attack;
    } while (attack & empty);

    // south ray
    attack = base;
    do {
	attack >>= 8;
	attacks |= attack;
    } while (attack & empty);

    // west ray
    attack = base; 
    do {
	attack >>= 1;
	attacks |= attack & rank_occ;
    } while (attack & empty & rank_occ);

    // east ray
    attack = base;
    do {
	attack <<= 1;
	attacks |= attack & rank_occ;
    } while (attack & empty & rank_occ);
    
    return attacks;
}

static u64 BishopAttacksOnFly(Square square, u64 occupancy) {
    u64 attacks = 0;
    u64 base = 1ULL << square;
    u64 empty = ~occupancy;

    u64 attack;

    // north-east ray
    attack = base; 
    do {
	attack = (attack << 9) & ~FILE_A;
	attacks |= attack;
    } while (attack & empty);

    // north-west ray
    attack = base; 
    do {
	attack = (attack << 7) & ~FILE_H;
	attacks |= attack;
    } while (attack & empty);

    // south-east ray
    attack = base; 
    do {
	attack = (attack >> 7) & ~FILE_A;
	attacks |= attack;
    } while (attack & empty);

    // south-west ray
    attack = base; 
    do {
	attack = (attack >> 9) & ~FILE_H;
	attacks |= attack;
    } while (attack & empty);

    return attacks;
}

//
// Initializing Magics
//

void InitMagics(void) {
    // initialize rooks and bishops at same time
    for (int square = 0; square < 64; square++) {
	int rook_bit_amt = ROOK_RELEVANT_BITS[square];
	int rook_subset_len = 1 << rook_bit_amt;

	int bishop_bit_amt = BISHOP_RELEVANT_BITS[square];
	int bishop_subset_len = 1 << bishop_bit_amt;

	// fills bit positions array from lsb to msb
	u64 rook_mask = ROOK_MASKS[square];
	int rook_bit_positions[rook_bit_amt];

	u64 bishop_mask = BISHOP_MASKS[square];
	int bishop_bit_positions[bishop_bit_amt];

	for (int i = 0; i < rook_bit_amt; i++) {
	    rook_bit_positions[i] = LsbIndex(rook_mask);
	    PopLsb(&rook_mask);
	}

	for (int i = 0; i < bishop_bit_amt; i++) {
	    bishop_bit_positions[i] = LsbIndex(bishop_mask);
	    PopLsb(&bishop_mask);
	}

	// Compress mask into contiguous int
	u64 rook_compressed_mask = rook_subset_len - 1;
	u64 bishop_compressed_mask = bishop_subset_len - 1;

	u64 rook_magic = ROOK_MAGICS[square];
	u64 bishop_magic = BISHOP_MAGICS[square];

	for (int compressed_subset = rook_compressed_mask; compressed_subset >= 0; compressed_subset--) {
	    // convert compressed subset into subset	
	    u64 subset = 0;

	    for (int i = 0; i < rook_bit_amt; i++) {
		subset |= (GetBit(compressed_subset, i) >> i) << rook_bit_positions[i];
	    }

	    u64 index = (subset * rook_magic) >> (64 - rook_bit_amt);
	    u64 attack = RookAttacksOnFly(square,subset);
	    
	    ROOK_ATTACKS[square][index] = attack;
	}

	for (int compressed_subset = bishop_compressed_mask; compressed_subset >= 0; compressed_subset--) {
	    // convert compressed subset into subset	
	    u64 subset = 0;

	    for (int i = 0; i < bishop_bit_amt; i++) {
		subset |= (GetBit(compressed_subset, i) >> i) << bishop_bit_positions[i];
	    }

	    u64 index = (subset * bishop_magic) >> (64 - bishop_bit_amt);
	    u64 attack = BishopAttacksOnFly(square,subset);
	    
	    BISHOP_ATTACKS[square][index] = attack;
	}
    }
}

//
// Generating Magic numbers
//

u64 GenMagicForSquare(Square square, bool is_rook, bool print) {
    u64 working_magic = 0ULL;
    int tries = 0;
    if (is_rook) {
	int bit_amt = ROOK_RELEVANT_BITS[square];
	int subset_len = 1 << bit_amt;

	// fills bit positions array from lsb to msb
	u64 mask = ROOK_MASKS[square];
	int bit_positions[bit_amt];

	for (int i = 0; i < bit_amt; i++) {
	    bit_positions[i] = LsbIndex(mask);
	    PopLsb(&mask);
	}

	// Compress mask into contiguous int
	u64 compressed_mask = subset_len - 1;

	u64 subsets[subset_len];
	u64 attack_masks[subset_len];

	for (int compressed_subset = compressed_mask; compressed_subset >= 0; compressed_subset--) {
	    // convert compressed subset into subset	
	    u64 subset = 0;

	    for (int i = 0; i < bit_amt; i++) {
		subset |= (GetBit(compressed_subset, i) >> i) << bit_positions[i];
	    }

	    // generate and store subset and attacks once
	    subsets[compressed_subset] = subset;
	    attack_masks[compressed_subset] = RookAttacksOnFly(square, subset);
	}

	// loop untill we find magic number that works
	while (1) {
	    // fill in table with nulls
	    for (int i = 0; i < subset_len; i++) {
		ROOK_ATTACKS[square][i] = NULL_BITBOARD;
	    }

	    // generate magic number to try
	    u64 magic = RandomU64() & RandomU64() & RandomU64();

	    // set fail to false and if theres bad collision we loop
	    bool fail = false;

	    // loop through array of attacks and subsets
	    for (int i = 0; i < subset_len; i++) {
		u64 attack_mask = attack_masks[i];
		u64 subset = subsets[i];
		u64 index = (magic * subset) >> (64 - bit_amt);

		u64 attack = ROOK_ATTACKS[square][index];
		if (attack == NULL_BITBOARD) {
		    ROOK_ATTACKS[square][index] = attack_mask;
		} else if (attack != attack_mask) {
		    // if bad collision
		    fail = true;
		    break;
		}
	    }

	    tries++;
	    if (!fail) {
		working_magic = magic;
		break;
	    }
	}
    } else {
	int bit_amt = BISHOP_RELEVANT_BITS[square];
	int subset_len = 1 << bit_amt;

	// fills bit positions array from lsb to msb
	u64 mask = BISHOP_MASKS[square];
	int bit_positions[bit_amt];

	for (int i = 0; i < bit_amt; i++) {
	    bit_positions[i] = LsbIndex(mask);
	    PopLsb(&mask);
	}

	// Compress mask into contiguous int
	u64 compressed_mask = subset_len - 1;

	u64 subsets[subset_len];
	u64 attack_masks[subset_len];

	for (int compressed_subset = compressed_mask; compressed_subset >= 0; compressed_subset--) {
	    // convert compressed subset into subset	
	    u64 subset = 0;

	    for (int i = 0; i < bit_amt; i++) {
		subset |= (GetBit(compressed_subset, i) >> i) << bit_positions[i];
	    }

	    // generate and store subset and attacks once
	    subsets[compressed_subset] = subset;
	    attack_masks[compressed_subset] = BishopAttacksOnFly(square, subset);
	}

	// loop untill we find magic number that works
	while (1) {
	    // fill in table with nulls
	    for (int i = 0; i < subset_len; i++) {
		BISHOP_ATTACKS[square][i] = NULL_BITBOARD;
	    }

	    // generate magic number to try
	    u64 magic = RandomU64() & RandomU64() & RandomU64();

	    // set fail to false and if theres bad collision we loop
	    bool fail = false;

	    // loop through array of attacks and subsets
	    for (int i = 0; i < subset_len; i++) {
		u64 attack_mask = attack_masks[i];
		u64 subset = subsets[i];
		u64 index = (magic * subset) >> (64 - bit_amt);

		u64 attack = BISHOP_ATTACKS[square][index];
		if (attack == NULL_BITBOARD) {
		    BISHOP_ATTACKS[square][index] = attack_mask;
		} else if (attack != attack_mask) {
		    // if bad collision
		    fail = true;
		    break;
		}
	    }

	    tries++;
	    if (!fail) {
		working_magic = magic;
		break;
	    }
	}
	// if bishop
	 
    }

    if (print) {
	if (working_magic != 0) {
	    printf("Found magic: 0x%" PRIx64 "ULL, for square %d\n",working_magic,square);
	    printf("Attempts: %d\n",tries);
	} else {
	    printf("Could not find magic for square %d\n",square);
	    printf("Attempts: %d\n",tries);
	}
    }

    return working_magic;
}

// gen all magics and print tables
void GenAllMagics() {
    printf("Rook magics: \n{\n");
    for (int i = 0; i < 64; i++) {
	if (i % 4 == 0 && i != 0) {
	    printf("\n");
	}
	u64 magic = GenMagicForSquare(i, true, false);
	printf("0x%" PRIx64 "ULL, ",magic);
    }
    printf("\n}\n");

    printf("\nBishop magics: \n{\n");
    for (int i = 0; i < 64; i++) {
	if (i % 4 == 0 && i != 0) {
	    printf("\n");
	}
	u64 magic = GenMagicForSquare(i, false, false);
	printf("0x%" PRIx64 "ULL, ",magic);
    }
    printf("\n}\n");
}
