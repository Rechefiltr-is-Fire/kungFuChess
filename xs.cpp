#pragma warning(disable : 26812)

#include <cstdint>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <bitset>
#include "xs.h"
//#include <pre/json/to_json.hpp>

namespace chess_xs {


//Inverts the color (WHITE -> BLACK) and (BLACK -> WHITE)
constexpr Color operator~(Color c) {
	return Color(c ^ BLACK);
}

Direction pawn_push(Color c) {
    if (c == WHITE) {
        return NORTH;
    }
    return SOUTH;
}

//All masks have been generated from a Java program

//Precomputed file masks
const Bitboard MASK_FILE[8] = {
	0x101010101010101, 0x202020202020202, 0x404040404040404, 0x808080808080808,
	0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
};

//Precomputed rank masks
const Bitboard MASK_RANK[8] = {
	0xff, 0xff00, 0xff0000, 0xff000000,
	0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000
};

//Precomputed diagonal masks
const Bitboard MASK_DIAGONAL[15] = {
	0x80, 0x8040, 0x804020,
	0x80402010, 0x8040201008, 0x804020100804,
	0x80402010080402, 0x8040201008040201, 0x4020100804020100,
	0x2010080402010000, 0x1008040201000000, 0x804020100000000,
	0x402010000000000, 0x201000000000000, 0x100000000000000,
};

//Precomputed anti-diagonal masks
const Bitboard MASK_ANTI_DIAGONAL[15] = {
	0x1, 0x102, 0x10204,
	0x1020408, 0x102040810, 0x10204081020,
	0x1020408102040, 0x102040810204080, 0x204081020408000,
	0x408102040800000, 0x810204080000000, 0x1020408000000000,
	0x2040800000000000, 0x4080000000000000, 0x8000000000000000,
};


//Shifts a bitboard in a particular direction. There is no wrapping, so bits that are shifted of the edge are lost
template<Direction D>
Bitboard shift(Bitboard b) {
    return D == NORTH ? b << 8 : D == SOUTH ? b >> 8
        : D == NORTH + NORTH ? b << 16 : D == SOUTH + SOUTH ? b >> 16
        : D == EAST ? (b & ~MASK_FILE[HFILE]) << 1 : D == WEST ? (b & ~MASK_FILE[AFILE]) >> 1
        : D == NORTH_EAST ? (b & ~MASK_FILE[HFILE]) << 9
        : D == NORTH_WEST ? (b & ~MASK_FILE[AFILE]) << 7
        : D == SOUTH_EAST ? (b & ~MASK_FILE[HFILE]) >> 7
        : D == SOUTH_WEST ? (b & ~MASK_FILE[AFILE]) >> 9
        : 0;
}

inline Square& operator++(Square& s) { return s = Square(int(s) + 1); }
constexpr Square operator+(Square s, Direction d) { return Square(int(s) + int(d)); }
constexpr Square operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
inline Square& operator+=(Square& s, Direction d) { return s = s + d; }
inline Square& operator-=(Square& s, Direction d) { return s = s - d; }

//A lookup table for king move bitboards
const Bitboard KING_ATTACKS[64] = {
	0x302, 0x705, 0xe0a, 0x1c14,
	0x3828, 0x7050, 0xe0a0, 0xc040,
	0x30203, 0x70507, 0xe0a0e, 0x1c141c,
	0x382838, 0x705070, 0xe0a0e0, 0xc040c0,
	0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00,
	0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000,
	0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000,
	0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000,
	0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000,
	0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000,
	0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000,
	0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 0xc040c000000000,
	0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000,
	0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
	0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000,
	0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000,
};

//A lookup table for knight move bitboards
const Bitboard KNIGHT_ATTACKS[64] = {
	0x20400, 0x50800, 0xa1100, 0x142200,
	0x284400, 0x508800, 0xa01000, 0x402000,
	0x2040004, 0x5080008, 0xa110011, 0x14220022,
	0x28440044, 0x50880088, 0xa0100010, 0x40200020,
	0x204000402, 0x508000805, 0xa1100110a, 0x1422002214,
	0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040,
	0x20400040200, 0x50800080500, 0xa1100110a00, 0x142200221400,
	0x284400442800, 0x508800885000, 0xa0100010a000, 0x402000204000,
	0x2040004020000, 0x5080008050000, 0xa1100110a0000, 0x14220022140000,
	0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000,
	0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000,
	0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
	0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000,
	0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
	0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000,
	0x44280000000000, 0x0088500000000000, 0x0010a00000000000, 0x20400000000000
};

//A lookup table for white pawn move bitboards
const Bitboard WHITE_PAWN_ATTACKS[64] = {
	0x200, 0x500, 0xa00, 0x1400,
	0x2800, 0x5000, 0xa000, 0x4000,
	0x20000, 0x50000, 0xa0000, 0x140000,
	0x280000, 0x500000, 0xa00000, 0x400000,
	0x2000000, 0x5000000, 0xa000000, 0x14000000,
	0x28000000, 0x50000000, 0xa0000000, 0x40000000,
	0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
	0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
	0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
	0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
	0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
	0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
	0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000,
	0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0,
};

//A lookup table for black pawn move bitboards
const Bitboard BLACK_PAWN_ATTACKS[64] = {
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0,
	0x2, 0x5, 0xa, 0x14,
	0x28, 0x50, 0xa0, 0x40,
	0x200, 0x500, 0xa00, 0x1400,
	0x2800, 0x5000, 0xa000, 0x4000,
	0x20000, 0x50000, 0xa0000, 0x140000,
	0x280000, 0x500000, 0xa00000, 0x400000,
	0x2000000, 0x5000000, 0xa000000, 0x14000000,
	0x28000000, 0x50000000, 0xa0000000, 0x40000000,
	0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
	0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
	0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
	0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
	0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
	0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
};

//Precomputed square masks
const Bitboard SQUARE_BB[65] = {
	0x1, 0x2, 0x4, 0x8,
	0x10, 0x20, 0x40, 0x80,
	0x100, 0x200, 0x400, 0x800,
	0x1000, 0x2000, 0x4000, 0x8000,
	0x10000, 0x20000, 0x40000, 0x80000,
	0x100000, 0x200000, 0x400000, 0x800000,
	0x1000000, 0x2000000, 0x4000000, 0x8000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000,
	0x100000000, 0x200000000, 0x400000000, 0x800000000,
	0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
	0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000,
	0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000,
	0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000,
	0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
	0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000,
	0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
	0x0
};

//Reverses a bitboard                        
Bitboard reverse(Bitboard b) {
	b = (b & 0x5555555555555555) << 1 | (b >> 1) & 0x5555555555555555;
	b = (b & 0x3333333333333333) << 2 | (b >> 2) & 0x3333333333333333;
	b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | (b >> 4) & 0x0f0f0f0f0f0f0f0f;
	b = (b & 0x00ff00ff00ff00ff) << 8 | (b >> 8) & 0x00ff00ff00ff00ff;

	return (b << 48) | ((b & 0xffff0000) << 16) |
		((b >> 16) & 0xffff0000) | (b >> 48);
}

//Calculates sliding attacks from a given square, on a given axis, taking into
//account the blocking pieces. This uses the Hyperbola Quintessence Algorithm.
Bitboard sliding_attacks(Square square, Bitboard occ, Bitboard mask) {
	return (((mask & occ) - SQUARE_BB[square] * 2) ^
		reverse(reverse(mask & occ) - reverse(SQUARE_BB[square]) * 2)) & mask;
}

//Returns rook attacks from a given square, using the Hyperbola Quintessence Algorithm. Only used to initialize
//the magic lookup table
Bitboard get_rook_attacks_for_init(Square square, Bitboard occ) {
	return sliding_attacks(square, occ, MASK_FILE[file_of(square)]) |
		sliding_attacks(square, occ, MASK_RANK[rank_of(square)]);
}

//Returns bishop attacks from a given square, using the Hyperbola Quintessence Algorithm. Only used to initialize
//the magic lookup table
Bitboard get_bishop_attacks_for_init(Square square, Bitboard occ) {
	return sliding_attacks(square, occ, MASK_DIAGONAL[diagonal_of(square)]) |
		sliding_attacks(square, occ, MASK_ANTI_DIAGONAL[anti_diagonal_of(square)]);
}

Bitboard ROOK_ATTACK_MASKS[64];
int ROOK_ATTACK_SHIFTS[64];
Bitboard ROOK_ATTACKS[64][4096];

const Bitboard ROOK_MAGICS[64] = {
	0x0080001020400080, 0x0040001000200040, 0x0080081000200080, 0x0080040800100080,
	0x0080020400080080, 0x0080010200040080, 0x0080008001000200, 0x0080002040800100,
	0x0000800020400080, 0x0000400020005000, 0x0000801000200080, 0x0000800800100080,
	0x0000800400080080, 0x0000800200040080, 0x0000800100020080, 0x0000800040800100,
	0x0000208000400080, 0x0000404000201000, 0x0000808010002000, 0x0000808008001000,
	0x0000808004000800, 0x0000808002000400, 0x0000010100020004, 0x0000020000408104,
	0x0000208080004000, 0x0000200040005000, 0x0000100080200080, 0x0000080080100080,
	0x0000040080080080, 0x0000020080040080, 0x0000010080800200, 0x0000800080004100,
	0x0000204000800080, 0x0000200040401000, 0x0000100080802000, 0x0000080080801000,
	0x0000040080800800, 0x0000020080800400, 0x0000020001010004, 0x0000800040800100,
	0x0000204000808000, 0x0000200040008080, 0x0000100020008080, 0x0000080010008080,
	0x0000040008008080, 0x0000020004008080, 0x0000010002008080, 0x0000004081020004,
	0x0000204000800080, 0x0000200040008080, 0x0000100020008080, 0x0000080010008080,
	0x0000040008008080, 0x0000020004008080, 0x0000800100020080, 0x0000800041000080,
	0x00FFFCDDFCED714A, 0x007FFCDDFCED714A, 0x003FFFCDFFD88096, 0x0000040810002101,
	0x0001000204080011, 0x0001000204000801, 0x0001000082000401, 0x0001FFFAABFAD1A2
};

Bitboard BISHOP_ATTACK_MASKS[64];
int BISHOP_ATTACK_SHIFTS[64];
Bitboard BISHOP_ATTACKS[64][512];

const Bitboard BISHOP_MAGICS[64] = {
	0x0002020202020200, 0x0002020202020000, 0x0004010202000000, 0x0004040080000000,
	0x0001104000000000, 0x0000821040000000, 0x0000410410400000, 0x0000104104104000,
	0x0000040404040400, 0x0000020202020200, 0x0000040102020000, 0x0000040400800000,
	0x0000011040000000, 0x0000008210400000, 0x0000004104104000, 0x0000002082082000,
	0x0004000808080800, 0x0002000404040400, 0x0001000202020200, 0x0000800802004000,
	0x0000800400A00000, 0x0000200100884000, 0x0000400082082000, 0x0000200041041000,
	0x0002080010101000, 0x0001040008080800, 0x0000208004010400, 0x0000404004010200,
	0x0000840000802000, 0x0000404002011000, 0x0000808001041000, 0x0000404000820800,
	0x0001041000202000, 0x0000820800101000, 0x0000104400080800, 0x0000020080080080,
	0x0000404040040100, 0x0000808100020100, 0x0001010100020800, 0x0000808080010400,
	0x0000820820004000, 0x0000410410002000, 0x0000082088001000, 0x0000002011000800,
	0x0000080100400400, 0x0001010101000200, 0x0002020202000400, 0x0001010101000200,
	0x0000410410400000, 0x0000208208200000, 0x0000002084100000, 0x0000000020880000,
	0x0000001002020000, 0x0000040408020000, 0x0004040404040000, 0x0002020202020000,
	0x0000104104104000, 0x0000002082082000, 0x0000000020841000, 0x0000000000208800,
	0x0000000010020200, 0x0000000404080200, 0x0000040404040400, 0x0002020202020200
};


const Bitboard k1 = 0x5555555555555555;
const Bitboard k2 = 0x3333333333333333;
const Bitboard k4 = 0x0f0f0f0f0f0f0f0f;
const Bitboard kf = 0x0101010101010101;

Bitboard LINE[64][64];

Bitboard SQUARES_BETWEEN_BB[64][64];

constexpr Piece make_piece(Color c, PieceType pt) {
  return Piece((c << 3) + pt);
}

constexpr PieceType type_of(Piece pc) {
  return PieceType(pc & 7);
}

Bitboard PAWN_ATTACKS[COLOR_NB][NSQUARES];
Bitboard PSEUDO_LEGAL_ATTACKS[PIECE_TYPE_NB][NSQUARES];

//Returns number of set bits in the bitboard
inline int pop_count(Bitboard x) {
	x = x - ((x >> 1) & k1);
	x = (x & k2) + ((x >> 2) & k2);
	x = (x + (x >> 4)) & k4;
	x = (x * kf) >> 56;
	return int(x);
}

//Returns number of set bits in the bitboard. Faster than pop_count(x) when the bitboard has few set bits
inline int sparse_pop_count(Bitboard x) {
	int count = 0;
	while (x) {
		count++;
		x &= x - 1;
	}
	return count;
}

const Bitboard index64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};

Bitboard MAGIC_BSF = 0x03f79d71b4cb0a89;

/**
 * bitScanForward
 * @author Martin Läuter (1997)
 *         Charles E. Leiserson
 *         Harald Prokop
 *         Keith H. Randall
 * "Using de Bruijn Sequences to Index a 1 in a Computer Word"
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
int bitScanForward(Bitboard bb) {
   assert (bb != 0);
   return index64[((bb & -bb) * MAGIC_BSF) >> 58];
}

//Returns the index of the least significant bit in the bitboard, and removes the bit from the bitboard
inline Square pop_lsb(Bitboard& b) {
    int lsb = bitScanForward(b);
    b &= b - 1;
    return Square(lsb);
}
//The type of the move
enum MoveFlags : int {
	QUIET = 0b0000, DOUBLE_PUSH = 0b0001,
	OO = 0b0010, OOO = 0b0011,
	CAPTURE = 0b1000,
	CAPTURES = 0b1111,
	EN_PASSANT = 0b1010,
	PROMOTIONS = 0b0111,
	PROMOTION_CAPTURES = 0b1100,
	PR_KNIGHT = 0b0100, PR_BISHOP = 0b0101, PR_ROOK = 0b0110, PR_QUEEN = 0b0111,
	PC_KNIGHT = 0b1100, PC_BISHOP = 0b1101, PC_ROOK = 0b1110, PC_QUEEN = 0b1111,
};

//const int SQUARE_NB = 65;

/// popcount() counts the number of non-zero bits in a bitboard
uint8_t PopCnt16[1 << 16];
uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];


/// xorshift64star Pseudo-Random Number Generator
/// This class is based on original code written and dedicated
/// to the public domain by Sebastiano Vigna (2014).
/// It has the following characteristics:
///
///  -  Outputs 64-bit numbers
///  -  Passes Dieharder and SmallCrush test batteries
///  -  Does not require warm-up, no zeroland to escape
///  -  Internal state is a single 64-bit integer
///  -  Period is 2^64 - 1
///  -  Speed: 1.60 ns/call (Core i7 @3.40GHz)
///
/// For further analysis see
///   <http://vigna.di.unimi.it/ftp/papers/xorshift.pdf>

class PRNG {

  uint64_t s;

  uint64_t rand64() {

    s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
    return s * 2685821657736338717LL;
  }

public:
  PRNG(uint64_t seed) : s(seed) { assert(seed); }

  template<typename T> T rand() { return T(rand64()); }

  /// Special generator used to fast init magic numbers.
  /// Output values only have 1/8th of their bits set on average.
  template<typename T> T sparse_rand()
  { return T(rand64() & rand64() & rand64()); }
};


// Magic holds all magic bitboards relevant data for a single square
struct Magic {
  Bitboard  mask;
  Bitboard  magic;
  Bitboard* attacks;
  unsigned  shift;

  // Compute the attack's index using the 'magic bitboards' approach
  unsigned index(Bitboard occupied) const {

    if (HasPext)
        return unsigned(pext(occupied, mask));

    if (Is64Bit)
        return unsigned(((occupied & mask) * magic) >> shift);

    unsigned lo = unsigned(occupied) & unsigned(mask);
    unsigned hi = unsigned(occupied >> 32) & unsigned(mask >> 32);
    return (lo * unsigned(magic) ^ hi * unsigned(magic >> 32)) >> shift;
  }
};

inline int popcount(Bitboard b) {

#ifndef USE_POPCNT

  union { Bitboard bb; uint16_t u[4]; } v = { b };
  return PopCnt16[v.u[0]] + PopCnt16[v.u[1]] + PopCnt16[v.u[2]] + PopCnt16[v.u[3]];

#elif defined(_MSC_VER) || defined(__INTEL_COMPILER)

  return (int)_mm_popcnt_u64(b);

#else // Assumed gcc or compatible compiler

  return __builtin_popcountll(b);

#endif
}


} // end namespace chess_xs

using namespace chess_xs;

//#undef Move
//#undef to
//class Move {
//private:
	////The internal representation of the move
	//uint16_t move;
//public:
	////Defaults to a null move (a1a1)
	//inline Move() : move(0) {}
	
	//inline Move(uint16_t m) { move = m; }

	//inline Move(Square from, Square to) : move(0) {
		//move = (from << 6) | to;
	//}

	//inline Move(Square from, Square to, MoveFlags flags) : move(0) {
		//move = (flags << 12) | (from << 6) | to;
	//}

    ////Move(const std::string& move) {
        ////this->move = (create_square(File(move[0] - 'a'), Rank(move[1] - '1')) << 6) |
            ////create_square(File(move[2] - 'a'), Rank(move[3] - '1'));
    ////}

    //inline Square to() const { return Square(move & 0x3f); }
	////inline Square from() const { return Square((move >> 6) & 0x3f); }
	////inline int to_from() const { return move & 0xffff; }
	////inline MoveFlags flags() const { return MoveFlags((move >> 12) & 0xf); }

	////inline bool is_capture() const {
		////return (move >> 12) & CAPTURES;
	////}

	////void operator=(Move m) { move = m.move; }
	////bool operator==(Move a) const { return to_from() == a.to_from(); }
	////bool operator!=(Move a) const { return to_from() != a.to_from(); }
//};

Move make_move(Square from, Square to, MoveFlags flags) {
    return (flags << 12) | (from << 6) | to;
}

// TODO make it faster like Stockfish?
bool is_endgame = false;
#define S(mg, eg) make_score(mg, eg)
int make_score(int mg, int eg) {
  return (is_endgame ? eg : mg);
}

//Initializes the magic lookup table for rooks
void initialise_rook_attacks() {
    Bitboard edges, subset, index;

    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq) {
        edges = ((MASK_RANK[AFILE] | MASK_RANK[HFILE]) & ~MASK_RANK[rank_of(sq)]) |
            ((MASK_FILE[AFILE] | MASK_FILE[HFILE]) & ~MASK_FILE[file_of(sq)]);
        ROOK_ATTACK_MASKS[sq] = (MASK_RANK[rank_of(sq)]
            ^ MASK_FILE[file_of(sq)]) & ~edges;
        ROOK_ATTACK_SHIFTS[sq] = 64 - pop_count(ROOK_ATTACK_MASKS[sq]);

        subset = 0;
        do {
            index = subset;
            index = index * ROOK_MAGICS[sq];
            index = index >> ROOK_ATTACK_SHIFTS[sq];
            ROOK_ATTACKS[sq][index] = get_rook_attacks_for_init(sq, subset);
            subset = (subset - ROOK_ATTACK_MASKS[sq]) & ROOK_ATTACK_MASKS[sq];
        } while (subset);
    }
}

//Initializes the magic lookup table for bishops
void initialise_bishop_attacks() {
	Bitboard edges, subset, index;

	for (Square sq = SQ_A1; sq <= SQ_H8; ++sq) {
		edges = ((MASK_RANK[AFILE] | MASK_RANK[HFILE]) & ~MASK_RANK[rank_of(sq)]) |
			((MASK_FILE[AFILE] | MASK_FILE[HFILE]) & ~MASK_FILE[file_of(sq)]);
		BISHOP_ATTACK_MASKS[sq] = (MASK_DIAGONAL[diagonal_of(sq)]
			^ MASK_ANTI_DIAGONAL[anti_diagonal_of(sq)]) & ~edges;
		BISHOP_ATTACK_SHIFTS[sq] = 64 - pop_count(BISHOP_ATTACK_MASKS[sq]);

		subset = 0;
		do {
			index = subset;
			index = index * BISHOP_MAGICS[sq];
			index = index >> BISHOP_ATTACK_SHIFTS[sq];
			BISHOP_ATTACKS[sq][index] = get_bishop_attacks_for_init(sq, subset);
			subset = (subset - BISHOP_ATTACK_MASKS[sq]) & BISHOP_ATTACK_MASKS[sq];
		} while (subset);
	}
}

//Initializes the lookup table for the bitboard of squares in between two given squares (0 if the 
//two squares are not aligned)
void initialise_squares_between() {
	Bitboard sqs;
	for (Square sq1 = SQ_A1; sq1 <= SQ_H8; ++sq1)
		for (Square sq2 = SQ_A1; sq2 <= SQ_H8; ++sq2) {
			sqs = SQUARE_BB[sq1] | SQUARE_BB[sq2];
			if (file_of(sq1) == file_of(sq2) || rank_of(sq1) == rank_of(sq2))
				SQUARES_BETWEEN_BB[sq1][sq2] =
				get_rook_attacks_for_init(sq1, sqs) & get_rook_attacks_for_init(sq2, sqs);
			else if (diagonal_of(sq1) == diagonal_of(sq2) || anti_diagonal_of(sq1) == anti_diagonal_of(sq2))
				SQUARES_BETWEEN_BB[sq1][sq2] =
				get_bishop_attacks_for_init(sq1, sqs) & get_bishop_attacks_for_init(sq2, sqs);
		}
}

//Initializes the table containg pseudolegal attacks of each piece for each square. This does not include blockers
//for sliding pieces
void initialise_pseudo_legal() {
	memcpy(PAWN_ATTACKS[WHITE], WHITE_PAWN_ATTACKS, sizeof(WHITE_PAWN_ATTACKS));
	memcpy(PAWN_ATTACKS[BLACK], BLACK_PAWN_ATTACKS, sizeof(BLACK_PAWN_ATTACKS));
	memcpy(PSEUDO_LEGAL_ATTACKS[KNIGHT], KNIGHT_ATTACKS, sizeof(KNIGHT_ATTACKS));
	memcpy(PSEUDO_LEGAL_ATTACKS[KING], KING_ATTACKS, sizeof(KING_ATTACKS));
	for (Square s = SQ_A1; s <= SQ_H8; ++s) {
		PSEUDO_LEGAL_ATTACKS[ROOK][s] = get_rook_attacks_for_init(s, 0);
		PSEUDO_LEGAL_ATTACKS[BISHOP][s] = get_bishop_attacks_for_init(s, 0);
		PSEUDO_LEGAL_ATTACKS[QUEEN][s] = PSEUDO_LEGAL_ATTACKS[ROOK][s] |
			PSEUDO_LEGAL_ATTACKS[BISHOP][s];
	}
}


//Initializes the lookup table for the bitboard of all squares along the line of two given squares (0 if the 
//two squares are not aligned)
void initialise_line() {
	for (Square sq1 = SQ_A1; sq1 <= SQ_H8; ++sq1)
		for (Square sq2 = SQ_A1; sq2 <= SQ_H8; ++sq2) {
			if (file_of(sq1) == file_of(sq2) || rank_of(sq1) == rank_of(sq2))
				LINE[sq1][sq2] =
				get_rook_attacks_for_init(sq1, 0) & get_rook_attacks_for_init(sq2, 0)
				| SQUARE_BB[sq1] | SQUARE_BB[sq2];
			else if (diagonal_of(sq1) == diagonal_of(sq2) || anti_diagonal_of(sq1) == anti_diagonal_of(sq2))
				LINE[sq1][sq2] =
				get_bishop_attacks_for_init(sq1, 0) & get_bishop_attacks_for_init(sq2, 0)
				| SQUARE_BB[sq1] | SQUARE_BB[sq2];
		}
}

/// distance() functions return the distance between x and y, defined as the
/// number of steps for a king in x to reach y.

int distanceFile(Square x, Square y) { return std::abs(file_of(x) - file_of(y)); }
int distanceRank(Square x, Square y) { return std::abs(rank_of(x) - rank_of(y)); }
int distanceSquare(Square x, Square y) { return SquareDistance[x][y]; }

//constexpr Square from_sq(Move m) {
  //return Square((m >> 6) & 0x3F);
//}

//constexpr Square to_sq(Move m) {
  //return Square(m & 0x3F);
//}

//constexpr int from_to(Move m) {
 //return m & 0xFFF;
//}

constexpr bool is_ok(Square s) {
  return s >= SQ_A1 && s <= SQ_H8;
}
//constexpr bool is_ok(Move m) {
  //return from_sq(m) != to_sq(m); // Catch MOVE_NULL and MOVE_NONE
//}

inline int edge_distance(File f) { return std::min(f, File(HFILE - f)); }
inline int edge_distance(Rank r) { return std::min(r, Rank(RANK_8 - r)); }

/// safe_destination() returns the bitboard of target square for the given step
/// from the given square. If the step is off the board, returns empty bitboard.

inline Bitboard safe_destination(Square s, int step) {
    Square to = Square(s + step);
    return is_ok(to) && distanceSquare(s, to) <= 2 ? square_bb(to) : Bitboard(0);
}

// init_magics() computes all rook and bishop attacks at startup. Magic
// bitboards are used to look up attacks of sliding pieces. As a reference see
// www.chessprogramming.org/Magic_Bitboards. In particular, here we use the so
// called "fancy" approach.

constexpr Bitboard rank_bb(Rank r) {
  return Rank1BB << (8 * r);
}

constexpr Bitboard rank_bb(Square s) {
  return rank_bb(rank_of(s));
}

constexpr Bitboard file_bb(File f) {
  return FileABB << f;
}

constexpr Bitboard file_bb(Square s) {
  return file_bb(file_of(s));
}

Bitboard sliding_attack(PieceType pt, Square sq, Bitboard occupied) {
    Bitboard attacks = 0;
    Direction   RookDirections[4] = {NORTH, SOUTH, EAST, WEST};
    Direction BishopDirections[4] = {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};

    for (Direction d : (pt == ROOK ? RookDirections : BishopDirections))
    {
        Square s = sq;
        while (safe_destination(s, d) && !(occupied & s))
            attacks |= (s += d);
    }

    //std::cout << "square: " << sq << ", attacks:" << attacks << "\n";
    return attacks;
}

Magic RookMagics[SQUARE_NB];
Magic BishopMagics[SQUARE_NB];

Bitboard RookTable[0x19000];  // To store rook attacks
Bitboard BishopTable[0x1480]; // To store bishop attacks

//Returns the attacks bitboard for a rook at a given square, using the magic lookup table
Bitboard get_rook_attacks(Square square, Bitboard occ) {
	return ROOK_ATTACKS[square][((occ & ROOK_ATTACK_MASKS[square]) * ROOK_MAGICS[square])
		>> ROOK_ATTACK_SHIFTS[square]];
}

//Returns the 'x-ray attacks' for a rook at a given square. X-ray attacks cover squares that are not immediately
//accessible by the rook, but become available when the immediate blockers are removed from the board 
Bitboard get_xray_rook_attacks(Square square, Bitboard occ, Bitboard blockers) {
	Bitboard attacks = get_rook_attacks(square, occ);
	blockers &= attacks;
	return attacks ^ get_rook_attacks(square, occ ^ blockers);
}

//Returns the attacks bitboard for a bishop at a given square, using the magic lookup table
Bitboard get_bishop_attacks(Square square, Bitboard occ) {
	return BISHOP_ATTACKS[square][((occ & BISHOP_ATTACK_MASKS[square]) * BISHOP_MAGICS[square])
		>> BISHOP_ATTACK_SHIFTS[square]];
}

//Returns a bitboard containing pawn attacks from all pawns in the given bitboard
Bitboard pawn_attacks(Color C, Bitboard p) {
    return C == WHITE ? shift<NORTH_WEST>(p) | shift<NORTH_EAST>(p) :
        shift<SOUTH_WEST>(p) | shift<SOUTH_EAST>(p);
}

//Bitboard pawn_attacks(Color C) {
    //return pawn_attacks(C, PAWN_BB[C]);
//}

//Returns a bitboard containing pawn attacks from the pawn on the given square
Bitboard pawn_attacks(Color C, Square s) {
    return PAWN_ATTACKS[C][s];
}

//Returns a bitboard containing pawn attacks from the pawn on the given square
Bitboard king_attacks(Square s) {
    return KING_ATTACKS[s];
}

//Returns the 'x-ray attacks' for a bishop at a given square. X-ray attacks cover squares that are not immediately
//accessible by the rook, but become available when the immediate blockers are removed from the board 
Bitboard get_xray_bishop_attacks(Square square, Bitboard occ, Bitboard blockers) {
	Bitboard attacks = get_bishop_attacks(square, occ);
	blockers &= attacks;
	return attacks ^ get_bishop_attacks(square, occ ^ blockers);
}


void init_magics(PieceType pt, Bitboard table[], Magic magics[]) {

    // Optimal PRNG seeds to pick the correct magics in the shortest time
    int seeds[][RANK_NB] = { { 8977, 44560, 54343, 38998,  5731, 95205, 104912, 17020 },
                             {  728, 10316, 55013, 32803, 12281, 15100,  16645,   255 } };

    Bitboard occupancy[4096], reference[4096], edges, b;
    int epoch[4096] = {}, cnt = 0, size = 0;

    for (Square s = SQ_A1; s <= SQ_H8; ++s)
    {
        // Board edges are not considered in the relevant occupancies
        edges = ((Rank1BB | Rank8BB) & ~rank_bb(s)) | ((FileABB | FileHBB) & ~file_bb(s));

        // Given a square 's', the mask is the bitboard of sliding attacks from
        // 's' computed on an empty board. The index must be big enough to contain
        // all the attacks for each possible subset of the mask and so is 2 power
        // the number of 1s of the mask. Hence we deduce the size of the shift to
        // apply to the 64 or 32 bits word to get the index.
        Magic& m = magics[s];
        m.mask  = sliding_attack(pt, s, 0) & ~edges;
        m.shift = (Is64Bit ? 64 : 32) - popcount(m.mask);

        // Set the offset for the attacks table of the square. We have individual
        // table sizes for each square with "Fancy Magic Bitboards".
        m.attacks = s == SQ_A1 ? table : magics[s - 1].attacks + size;

        // Use Carry-Rippler trick to enumerate all subsets of masks[s] and
        // store the corresponding sliding attack bitboard in reference[].
        b = size = 0;
        do {
            occupancy[size] = b;
            reference[size] = sliding_attack(pt, s, b);

            if (HasPext)
                m.attacks[pext(b, m.mask)] = reference[size];

            size++;
            b = (b - m.mask) & m.mask;
        } while (b);

        if (HasPext)
            continue;

        PRNG rng(seeds[Is64Bit][rank_of(s)]);

        // Find a magic for square 's' picking up an (almost) random number
        // until we find the one that passes the verification test.
        for (int i = 0; i < size; )
        {
            for (m.magic = 0; popcount((m.magic * m.mask) >> 56) < 6; ) {
                m.magic = rng.sparse_rand<Bitboard>();
            }

            // A good magic must map every possible occupancy to an index that
            // looks up the correct sliding attack in the attacks[s] database.
            // Note that we build up the database for square 's' as a side
            // effect of verifying the magic. Keep track of the attempt count
            // and save it in epoch[], little speed-up trick to avoid resetting
            // m.attacks[] after every failed attempt.
            for (++cnt, i = 0; i < size; ++i)
            {
                unsigned idx = m.index(occupancy[i]);

                if (epoch[idx] < cnt)
                {
                    epoch[idx] = cnt;
                    m.attacks[idx] = reference[i];
                }
                else if (m.attacks[idx] != reference[i])
                    break;
            }
        }
    }
}

Bitboard attacks_bb(PieceType pt, Square s, Bitboard occupied) {

  assert((pt != PAWN) && (is_ok(s)));

  switch (pt)
  {
  case BISHOP: return BishopMagics[s].attacks[BishopMagics[s].index(occupied)];
  case ROOK  : return   RookMagics[s].attacks[  RookMagics[s].index(occupied)];
  case QUEEN : return attacks_bb(BISHOP, s, occupied) | attacks_bb(ROOK, s, occupied);
  default    : return PseudoAttacks[pt][s];
  }
}

void initialise_bitboard() {

  for (unsigned i = 0; i < (1 << 16); ++i)
      PopCnt16[i] = uint8_t(std::bitset<16>(i).count());

  for (Square s = SQ_A1; s <= SQ_H8; ++s)
      SquareBB[s] = (1ULL << s);

  for (Square s1 = SQ_A1; s1 <= SQ_H8; ++s1) {
      for (Square s2 = SQ_A1; s2 <= SQ_H8; ++s2) {
          SquareDistance[s1][s2] = std::max(distanceFile(s1, s2), distanceRank(s1, s2));
          //std::cout << std::max(distanceFile(s1, s2), distanceRank(s1, s2)) << ":" << s1 << " vs " << s2 << "\n";
      }
  }

  init_magics(ROOK, RookTable, RookMagics);
  init_magics(BISHOP, BishopTable, BishopMagics);

  for (Square s1 = SQ_A1; s1 <= SQ_H8; ++s1)
  {
      PawnAttacks[WHITE][s1] = pawn_attacks(WHITE, square_bb(s1));
      PawnAttacks[BLACK][s1] = pawn_attacks(BLACK, square_bb(s1));

      for (int step : {-9, -8, -7, -1, 1, 7, 8, 9} )
         PseudoAttacks[KING][s1] |= safe_destination(s1, step);

      for (int step : {-17, -15, -10, -6, 6, 10, 15, 17} )
         PseudoAttacks[KNIGHT][s1] |= safe_destination(s1, step);

      PseudoAttacks[QUEEN][s1]  = PseudoAttacks[BISHOP][s1] = attacks_bb(BISHOP, s1, 0);
      PseudoAttacks[QUEEN][s1] |= PseudoAttacks[  ROOK][s1] = attacks_bb(ROOK, s1, 0);

      for (PieceType pt : { BISHOP, ROOK })
          for (Square s2 = SQ_A1; s2 <= SQ_H8; ++s2)
          {
              if (PseudoAttacks[pt][s1] & s2)
              {
                  LineBB[s1][s2]    = (attacks_bb(pt, s1, 0) & attacks_bb(pt, s2, 0)) | s1 | s2;
                  BetweenBB[s1][s2] = (attacks_bb(pt, s1, square_bb(s2)) & attacks_bb(pt, s2, square_bb(s1)));
              }
              BetweenBB[s1][s2] |= s2;
          }
  }
}


//Initializes lookup tables for rook moves, bishop moves, in-between squares, aligned squares and pseudolegal moves
void initialise_all_databases() {
    initialise_bitboard();
	initialise_rook_attacks();
	initialise_bishop_attacks();
	initialise_squares_between();
	initialise_line();
	initialise_pseudo_legal();
}
Bitboard pawns   = 0x0;
Bitboard knights = 0x0;
Bitboard bishops = 0x0;
Bitboard rooks   = 0x0;
Bitboard queens  = 0x0;
Bitboard kings   = 0x0;

Bitboard white  = 0x0;
Bitboard black  = 0x0;
Bitboard frozen = 0x0;
Bitboard moving = 0x0;

// attacks
Bitboard PAWN_ATTACKERS[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard PIECE_ATTACKERS[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard ROOK_ATTACKERS[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard QUEEN_ATTACKERS[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard KING_ATTACKERS[3] = {
    0x0,
    0x0,
    0x0
};

// pieces
Bitboard PAWN_BB[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard KNIGHT_BB[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard BISHOP_BB[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard ROOK_BB[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard QUEEN_BB[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard KING_BB[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard byColorBB[3] = {
    0x0,
    0x0,
    0x0
};
Bitboard byTypeBB[PIECE_TYPE_NB];

//#define moveSet int[6];

const int MOVE_FR       = 0;
const int MOVE_TO       = 1;
const int MOVE_SCORE    = 2;
const int MOVE_DISTANCE = 3;

// technically you can have more than 195 legal moves but whatever
typedef int (*MoveList)[195][5] ;

Piece board[65];
int moveSpot = 0;
std::vector<Move> moveArray(195);

Piece piece_on(Square sq) {
    return board[sq];
}

Move getNextMove() {
    std::cout << "getNextMove\n";
    if (moveSpot >= moveArray.size()) {
        return 0;
    } else {
        moveSpot++;
        return moveArray[moveSpot];
    }

}

// similar to Evaluation::pieces() in stockfish
void setAllMoves() {

    moveSpot = 0;
    std::cout << "getAllMoves()";
    std::cout << "\n";
    for (Color c : { WHITE, BLACK }) {
        std::cout << "color: " << c << "\n";

        Bitboard b = 0x0;

        Color Us   =  c;
        Color Them = ~c;

        //*********************** pawns
        b = PAWN_BB[c];
        std::cout << "pawn bb: " << b << "\n";
        while (Square sq = pop_lsb(b)) {
            std::cout << "  pawn sq: " << sq << "\n";
            Bitboard myMoves = pawn_attacks(Us, sq);
            Piece piece = make_piece(c, PAWN);
            // TODO how does this work? attacks only?
            PAWN_ATTACKERS[Us] |= myMoves;
            board[sq] = piece;
            while (Square sq_to = pop_lsb(myMoves)) {
                Move m = make_move(sq, sq_to, QUIET);
                moveArray.push_back(m);
            }
        }
        
        //*********************** kings
        b = KING_BB[c];
        std::cout << "king bb: " << b << "\n";
        while (Square sq = pop_lsb(b)) {
            std::cout << "  king sq: " << sq << "\n";
            Bitboard myMoves = king_attacks(sq);
            Piece piece = make_piece(c, KING);
            board[sq] = piece;
            while (Square sq_to = pop_lsb(myMoves)) {
                Move m = make_move(sq, sq_to, QUIET);
                moveArray.push_back(m);
            }
        }
        
        //*********************** bishops
        b = BISHOP_BB[c];
        std::cout << "bish bb: " << b << "\n";
        while (Square sq = pop_lsb(b)) {
            std::cout << "  bish sq: " << sq << "\n";
            Bitboard att_bb = get_bishop_attacks(sq, white & black);
            PIECE_ATTACKERS[Us] |= att_bb;
            std::cout << "bish att: " << att_bb << "\n";
            Piece piece = make_piece(c, BISHOP);
            board[sq] = piece;
            while (Square sq_to = pop_lsb(att_bb)) {
                Move m = make_move(sq, sq_to, QUIET);
                moveArray.push_back(m);
            }
        }
        
        //*********************** rooks
        b = ROOK_BB[c];
        std::cout << "rook bb: " << b << "\n";
        while (Square sq = pop_lsb(b)) {
            std::cout << "  rook sq: " << sq << "\n";
            Bitboard att_bb = get_rook_attacks(sq, white & black);
            std::cout << "rook att: " << att_bb << "\n";
            ROOK_ATTACKERS[Us] |= att_bb;
            Piece piece = make_piece(c, ROOK);
            board[sq] = piece;
            while (Square sq_to = pop_lsb(att_bb)) {
                Move m = make_move(sq, sq_to, QUIET);
                moveArray.push_back(m);
            }
        }
    
        //*********************** queens
        std::cout << "queen bb: " << b << "\n";
        b = QUEEN_BB[c];
        while (Square sq = pop_lsb(b)) {
            std::cout << "queen sq: " << b << "\n";
            Bitboard att_bb = get_rook_attacks(sq, white & black) | get_bishop_attacks(sq, white & black);
            std::cout << "queen att: " << att_bb << "\n";
            QUEEN_ATTACKERS[Us] |= att_bb;
            Piece piece = make_piece(c, QUEEN);
            board[sq] = piece;
            while (Square sq_to = pop_lsb(att_bb)) {
                Move m = make_move(sq, sq_to, QUIET);
                moveArray.push_back(m);
            }
        }
    }
}


const int NB_SQUARES = 64;
const int NB_PIECES  = 8;

int material[2];
int mobility[2];
int threats [2];

Bitboard attackedBy[3][NB_PIECES];
Bitboard attackedBy2[3];
Bitboard kingRing[3];
Bitboard mobilityArea[3];
int kingAttackersCount[3];
int kingAttackersWeight[3];
int kingAttacksCount[3];

// ThreatByMinor/ByRook[attacked PieceType] contains bonuses according to
// which piece type attacks which one. Attacks on lesser pieces which are
// pawn-defended are not considered.
int ThreatByMinor[PIECE_TYPE_NB] = {
    S(0, 0), S(5, 32), S(55, 41), S(77, 56), S(89, 119), S(79, 162)
};

int ThreatByRook[PIECE_TYPE_NB] = {
    S(0, 0), S(3, 44), S(37, 68), S(42, 60), S(0, 39), S(58, 43)
};
  // Assorted bonuses and penalties
int UncontestedOutpost  = S(  1, 10);
int BishopOnKingRing    = S( 24,  0);
int BishopXRayPawns     = S(  4,  5);
int FlankAttacks        = S(  8,  0);
int Hanging             = S( 69, 36);
int KnightOnQueen       = S( 16, 11);
int LongDiagonalBishop  = S( 45,  0);
int MinorBehindPawn     = S( 18,  3);
int PassedFile          = S( 11,  8);
int PawnlessFlank       = S( 17, 95);
int ReachableOutpost    = S( 31, 22);
int RestrictedPiece     = S(  7,  7);
int RookOnKingRing      = S( 16,  0);
int SliderOnQueen       = S( 60, 18);
int ThreatByKing        = S( 24, 89);
int ThreatByPawnPush    = S( 48, 39);
int ThreatBySafePawn    = S(173, 94);
int TrappedRook         = S( 55, 13);
int WeakQueenProtection = S( 14,  0);
int WeakQueen           = S( 56, 15);

// copied as much as possible from Stockfish
int evaluateThreats(Color Us) {
    Color Them = ~Us;
    //constexpr Direction Up       = pawn_push(Us);
    //constexpr Bitboard  TRank3BB = (Us == WHITE ? Rank3BB : Rank6BB);
    Bitboard b, weak, defended, nonPawnEnemies, stronglyProtected, safe;
    int score = 0;

    // Non-pawn enemies
    nonPawnEnemies = byColorBB[Them] & ~PAWN_BB[Them];

    // Squares strongly protected by the enemy, either because they defend the
    // square with a pawn, or because they defend the square twice and we don't.
    stronglyProtected =  attackedBy[Them][PAWN]
                       | (attackedBy2[Them] & ~attackedBy2[Us]);

    // Non-pawn enemies, strongly protected
    defended = nonPawnEnemies & stronglyProtected;

    // Enemies not strongly protected and under our attack
    weak = byColorBB[Them] & ~stronglyProtected & attackedBy[Us][ALL_PIECES];

    // Bonus according to the kind of attacking pieces
    if (defended | weak)
    {
        b = (defended | weak) & (attackedBy[Us][KNIGHT] | attackedBy[Us][BISHOP]);
        while (b)
            score += ThreatByMinor[type_of(piece_on(pop_lsb(b)))];

        b = weak & attackedBy[Us][ROOK];
        while (b)
            score += ThreatByRook[type_of(piece_on(pop_lsb(b)))];

        if (weak & attackedBy[Us][KING])
            score += ThreatByKing;

        b =  ~attackedBy[Them][ALL_PIECES]
           | (nonPawnEnemies & attackedBy2[Us]);
        score += Hanging * popcount(weak & b);

        // Additional bonus if weak piece is only protected by a queen
        score += WeakQueenProtection * popcount(weak & attackedBy[Them][QUEEN]);
    }

    // Bonus for restricting their piece moves
    b =   attackedBy[Them][ALL_PIECES]
       & ~stronglyProtected
       &  attackedBy[Us][ALL_PIECES];
    score += RestrictedPiece * popcount(b);

    // Protected or unattacked squares
    safe = ~attackedBy[Them][ALL_PIECES] | attackedBy[Us][ALL_PIECES];

    // Bonus for attacking enemy pieces with our relatively safe pawns
    b = PAWN_BB[Us] & safe;
    b = PAWN_ATTACKERS[Us] & nonPawnEnemies;
    score += ThreatBySafePawn * popcount(b);

    std::cout << "eval threats for " << Us << ": " << score << "\n";
    return score;
}

//int pieces(color c, piecetype pt) {
    //attackedby[us][pt] = 0;

//}

//template<Direction D>
Bitboard shift(Direction D, Bitboard b) {
  return  D == NORTH      ?  b             << 8 : D == SOUTH      ?  b             >> 8
        : D == NORTH+NORTH?  b             <<16 : D == SOUTH+SOUTH?  b             >>16
        : D == EAST       ? (b & ~FileHBB) << 1 : D == WEST       ? (b & ~FileABB) >> 1
        : D == NORTH_EAST ? (b & ~FileHBB) << 9 : D == NORTH_WEST ? (b & ~FileABB) << 7
        : D == SOUTH_EAST ? (b & ~FileHBB) >> 7 : D == SOUTH_WEST ? (b & ~FileABB) >> 9
        : 0;
}

Bitboard pieces(PieceType pt = ALL_PIECES) {
  return byTypeBB[pt];
}

Bitboard pieces(PieceType pt1, PieceType pt2) {
  return pieces(pt1) | pieces(pt2);
}

Bitboard pieces(Color c) {
  return byColorBB[c];
}

Bitboard pieces(Color c, PieceType pt) {
  return pieces(c) & pieces(pt);
}

Bitboard pieces(Color c, PieceType pt1, PieceType pt2) {
  return pieces(c) & (pieces(pt1) | pieces(pt2));
}


void evalInit(Color Us) {

    Color     Them = ~Us;
    Direction Up   = pawn_push(Us);
    Direction Down = pawn_push(Them);
    Bitboard LowRanks = (Us == WHITE ? Rank2BB | Rank3BB : Rank7BB | Rank6BB);

    Bitboard b = KING_BB[Us];
    Square ksq = pop_lsb(b);

    //Bitboard dblAttackByPawn = pawn_double_attacks_bb<Us>(pos.pieces(Us, PAWN));

    //// Find our pawns that are blocked or on the first two ranks
    //Bitboard b = pos.pieces(Us, PAWN) & (shift<Down>(pos.pieces()) | LowRanks);
    b = pieces(Us, PAWN) & (shift(Down, pieces()) | LowRanks);

    //// Squares occupied by those pawns, by our king or queen, by blockers to attacks on our king
    //// or controlled by enemy pawns are excluded from the mobility area.
    //mobilityArea[Us] = ~(b | pos.pieces(Us, KING, QUEEN) | pos.blockers_for_king(Us) | pe->pawn_attacks(Them));
    mobilityArea[Us] = ~(b | pieces(Us, KING, QUEEN) | pawn_attacks(Them, PAWN_BB[Them]));

    //// Initialize attackedBy[] for king and pawns
    //attackedBy[Us][KING] = attacks_bb<KING>(ksq);
    //attackedBy[Us][PAWN] = pe->pawn_attacks(Us);
    //attackedBy[Us][ALL_PIECES] = attackedBy[Us][KING] | attackedBy[Us][PAWN];
    //attackedBy2[Us] = dblAttackByPawn | (attackedBy[Us][KING] & attackedBy[Us][PAWN]);

    //// Init our king safety tables
    //Square s = make_square(std::clamp(file_of(ksq), FILE_B, FILE_G),
                           //std::clamp(rank_of(ksq), RANK_2, RANK_7));
    //kingRing[Us] = attacks_bb<KING>(s) | s;

    //kingAttackersCount[Them] = popcount(kingRing[Us] & pe->pawn_attacks(Them));
    //kingAttacksCount[Them] = kingAttackersWeight[Them] = 0;

    //// Remove from kingRing[] the squares defended by two pawns
    //kingRing[Us] &= ~dblAttackByPawn;
}

int evaluate() {
    int score = 0;
    std::cout << "cpp evaluate()\n";
    for (Color c : { WHITE, BLACK }) {
        Color Us   = c;
        Color Them = ~c;

        //std::cout << "eval for " << Us << ":" << Them << "\n";

        Bitboard OutpostRanks =
            (Us == WHITE ? Rank4BB | Rank5BB | Rank6BB
                         : Rank5BB | Rank4BB | Rank3BB);
        Bitboard b1;
        //------------------- pieces
        b1 = byColorBB[Us];

        while (b1)
        {
            Square s = pop_lsb(b1);
            PieceType Pt = type_of(piece_on(s));
            //std::cout << "piece type: " << piece_on(s) << " type: " << Pt << "\n";
        }

        score += evaluateThreats(c);
    }

    return score;
}


void setBBs(
    Bitboard bb_pawns,
    Bitboard bb_knights,
    Bitboard bb_bishops,
    Bitboard bb_rooks,
    Bitboard bb_queens,
    Bitboard bb_kings,
    Bitboard bb_white,
    Bitboard bb_black,
    Bitboard bb_frozen,
    Bitboard bb_moving
        ){
    pawns   = bb_pawns;
    PAWN_BB[WHITE] = bb_pawns & bb_white;
    PAWN_BB[BLACK] = bb_pawns & bb_black;
    byTypeBB[PAWN] = bb_pawns;

    knights = bb_knights;
    KNIGHT_BB[WHITE] = bb_knights & bb_white;
    KNIGHT_BB[BLACK] = bb_knights & bb_black;
    byTypeBB[KNIGHT] = bb_knights;

    bishops = bb_bishops;
    BISHOP_BB[WHITE] = bb_bishops & bb_white;
    BISHOP_BB[BLACK] = bb_bishops & bb_black;
    byTypeBB[BISHOP] = bb_bishops;

    rooks   = bb_rooks;
    ROOK_BB[WHITE] = bb_rooks & bb_white;
    ROOK_BB[BLACK] = bb_rooks & bb_black;
    byTypeBB[ROOK] = bb_rooks;

    queens  = bb_queens;
    QUEEN_BB[WHITE] = bb_queens & bb_white;
    QUEEN_BB[BLACK] = bb_queens & bb_black;
    byTypeBB[QUEEN] = bb_queens;

    kings   = bb_kings;
    KING_BB[WHITE] = bb_kings & bb_white;
    KING_BB[BLACK] = bb_kings & bb_black;
    byTypeBB[KING] = bb_kings;

    white   = bb_white;
    black   = bb_black;
    byColorBB[WHITE] = bb_white;
    byColorBB[BLACK] = bb_black;

    frozen  = bb_frozen;
    moving  = bb_moving;
}

constexpr Square make_square(File f, Rank r) {
  return Square((r << 3) + f);
}

/// Bitboards::pretty() returns an ASCII representation of a bitboard suitable
/// to be printed to standard output. Useful for debugging.

std::string pretty(Bitboard b) {

  std::string s = "+---+---+---+---+---+---+---+---+\n";

  for (Rank r = RANK_8; r >= RANK_1; r = r - 1)
  {
      for (File f = AFILE; f <= HFILE; f = f + 1)
          s += b & make_square(f, r) ? "| X " : "|   ";

      s += "| " + std::to_string(1 + r) + "\n+---+---+---+---+---+---+---+---+\n";
  }
  s += "  a   b   c   d   e   f   g   h\n";

  return s;
}
