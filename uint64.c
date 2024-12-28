#include "chess_engine.h"

uint64_t bitwise_and(uint64_t *a, uint64_t *b)
{
    uint64_t result = { 0u, 0u };
    result.low = a->low & b->low;
    result.high = a->high & b->high;
    return result;
}

uint64_t bitwise_or(uint64_t *a, uint64_t *b)
{
    uint64_t result = { 0u, 0u };
    result.low = a->low | b->low;
    result.high = a->high | b->high;
    return result;
}

void bitwise_left_shift(uint64_t *result, const short int shift)
{
    if (shift >= 32)
    {
        result->high = result->low << (shift - 32);
        result->low = 0u;
    } else
    {
        result->high = (result->high << shift) | (result->low >> (32 - shift));
        result->low = (result->low << shift);
    }
}

void bitwise_right_shift(uint64_t *result, const short int shift)
{
    if (shift >= 32)
    {
        result->low = result->high >> (shift - 32);
        result->high = 0u;
    } else
    {
        /* result->low = (result->low >> shift) | (result->high >> (32 - shift)); */
        result->low = (result->low >> shift) | ((result->high & ((1u << shift) - 1)) << (32 - shift));
        result->high = (result->high >> shift);
    }
}

void add(uint64_t *result, const uint64_t *a)
{
    unsigned int carry;
    result->low += a->low;
    carry = (result->low < a->low) ? 1 : 0;
    result->high += result->high + a->high + carry;
}

bool is_bit_set(piece_color color, piece_type type, int idx)
{
    uint64_t *bitboard = get_bitboard(color, type);
    if (idx < 32)
    {
        return (bitboard->low & (1u << idx)) != 0;
    }
    else
    {
        return (bitboard->high & (1u << (idx - 32))) != 0;
    }
}

void clear_bit(piece_color color, piece_type type, int idx)
{
    uint64_t *bitboard = get_bitboard(color, type);
    if (idx < 32) {bitboard->low &= ~(1u << idx);}
    else {bitboard->high &= ~(1u << (idx - 32));}
}

void set_bit(piece_color color, piece_type type, int idx)
{
    uint64_t *bitboard = get_bitboard(color, type);
    if (idx < 32) {bitboard->low |= (1u << idx);}
    else {bitboard->high |= (1u << (idx - 32));}
}

int count_set_bits(piece_color color, piece_type type)
{
    uint64_t *bitboard = get_bitboard(color, type);
    int mask = 1, count = 0, shift;
    for (shift = 0; shift < 32; shift++) { if (bitboard->low & mask) { count++; } mask <<= 1; }
    mask = 1;
    for (shift = 0; shift < 32; shift++) { if (bitboard->high & mask) { count++; } mask <<= 1; }

    return count;
}

