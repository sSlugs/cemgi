#include "movegen/magics.h"
#include "globals.h"
#include "utils/types.h"
#include "utils/bitboard.h"
#include <stdio.h>

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
0x8080016210804001ULL, 0x9040200110004000ULL, 0x1080100120002880ULL, 0x9080100080080006ULL, 
0xc200088200a47020ULL, 0x700040041001208ULL, 0x1080108001004200ULL, 0x10002102082c900ULL, 
0x404a800080400120ULL, 0xc040002000d000ULL, 0x2001880220040ULL, 0x40a004022004810ULL, 
0x10008020c1100ULL, 0x10801200802c01ULL, 0x1804008412100801ULL, 0x4802000104204082ULL, 
0x304281800020c000ULL, 0x441002402004c000ULL, 0x1000808010082000ULL, 0xb2210010008901ULL, 
0x200080800c002800ULL, 0x90080800a000400ULL, 0x1000040012080110ULL, 0x124820001088244ULL, 
0x4000401480008020ULL, 0x421008d00400060ULL, 0x4a200080100180ULL, 0x11022100100108ULL, 
0x4000080080140080ULL, 0x30c0080220080ULL, 0x8001000100520004ULL, 0x89c10c2200008849ULL, 
0x24010808000a2ULL, 0x100802000804000ULL, 0x2a4402001001300ULL, 0x86800800801000ULL, 
0x504040280800800ULL, 0x2202800200800400ULL, 0x418800100800200ULL, 0x842a124820000c4ULL, 
0x100c24c008848001ULL, 0x810005020084005ULL, 0x9002002041820016ULL, 0x900c810010021ULL, 
0x4481001148010004ULL, 0x2006040002008080ULL, 0x110d10a804000aULL, 0x1008404420001ULL, 
0x9401024800280ULL, 0x4022200080400080ULL, 0xc10040800200260ULL, 0x40180110028080ULL, 
0x2002050448a00ULL, 0x60104000a008080ULL, 0x2418100802012400ULL, 0x20171044088200ULL, 
0x802012715004082ULL, 0x40008301c4620012ULL, 0x8000b2a000410901ULL, 0x100200c089001ULL, 
0x22002045081002ULL, 0x2002104281002ULL, 0x1103088120104ULL, 0x20400c008028d102ULL, 
};
const u64 BISHOP_MAGICS[64] = {
0x82544118020780ULL, 0x15240082020002ULL, 0x2004412202010631ULL, 0x41c240080008800ULL, 
0x904104428000280ULL, 0x2003101210440021ULL, 0x42844402400000ULL, 0x10324310101010ULL, 
0x4000401004010040ULL, 0x14c441004010022ULL, 0x252c102404c4404bULL, 0x500081608400100ULL, 
0x40020404200000c0ULL, 0x800201100a5400ULL, 0x82a02014c204c08ULL, 0x2128c404118820ULL, 
0xc8c2001010320082ULL, 0x230891210420880ULL, 0x8050000518420141ULL, 0x4008000222004208ULL, 
0x4024042880a01020ULL, 0xa001000200808410ULL, 0x1000b48461020ULL, 0x53283020a410804ULL, 
0xa034040040380881ULL, 0x8122005040805ULL, 0x80010100500c0024ULL, 0x22008088008027ULL, 
0x8021010000104010ULL, 0x88002266008401ULL, 0xa0808002080480ULL, 0x110810091610800ULL, 
0x4484400c041000ULL, 0x90160210000a100aULL, 0x24002800940440ULL, 0x38200802050104ULL, 
0x4090020200312008ULL, 0x4240810220a0090ULL, 0x840100403c040aULL, 0x208450101014050ULL, 
0x843420104208ULL, 0x804008808205400ULL, 0x502010643004800ULL, 0x1e0204240200ULL, 
0x4008882100408c06ULL, 0x4082240102004c08ULL, 0x10d00204802060ULL, 0xa080250420900c0ULL, 
0x42414044c2101ULL, 0x808401200234ULL, 0x8000004404044010ULL, 0x110100820880020ULL, 
0x1080204018220042ULL, 0x45006020558ULL, 0x4040450404004010ULL, 0x6004018204010000ULL, 
0x200622110086010ULL, 0x8040020300a25031ULL, 0x12308020ca209000ULL, 0x12003c0002050400ULL, 
0x8300000640d0400ULL, 0x400000488900500ULL, 0x80810040060ULL, 0x8010200204002020ULL, 
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
