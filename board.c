#include "chess_engine.h"

Bitboard *bitboards = NULL;
int *board_array = NULL;

void init_boards()
{
    bitboards = malloc(sizeof(Bitboard));
    board_array = malloc(64 * sizeof(int));

    if (bitboards == NULL) { printf("Allocation for bitboards or board failed. \n"); exit(1); }
    else
    {
        uint64_t white_pawns   = { 0x0000FF00u, 0x00000000u };
        uint64_t white_knights = { 0x00000042u, 0x00000000u };
        uint64_t white_bishops = { 0x00000024u, 0x00000000u };
        uint64_t white_rooks   = { 0x00000081u, 0x00000000u };
        uint64_t white_queens  = { 0x00000010u, 0x00000000u };
        uint64_t white_kings   = { 0x00000008u, 0x00000000u };

        uint64_t black_pawns   = { 0x00000000u, 0x00FF0000u };
        uint64_t black_knights = { 0x00000000u, 0x42000000u };
        uint64_t black_bishops = { 0x00000000u, 0x24000000u };
        uint64_t black_rooks   = { 0x00000000u, 0x81000000u };
        uint64_t black_queens  = { 0x00000000u, 0x10000000u };
        uint64_t black_kings   = { 0x00000000u, 0x08000000u };

        bitboards->white_pawns = white_pawns;
        bitboards->white_knights = white_knights;
        bitboards->white_bishops = white_bishops;
        bitboards->white_rooks = white_rooks;
        bitboards->white_queens = white_queens;
        bitboards->white_kings = white_kings;

        bitboards->black_pawns = black_pawns;
        bitboards->black_knights = black_knights;
        bitboards->black_bishops = black_bishops;
        bitboards->black_rooks = black_rooks;
        bitboards->black_queens = black_queens;
        bitboards->black_kings = black_kings;

        clear_board();
        update_board();
    }
}

uint64_t *get_bitboard(piece_color color, piece_type type)
{
    uint64_t *fields = (uint64_t *) bitboards;
    return &fields[6 * color + type];
}

void print_bitboard(piece_color color, piece_type type)
{
    uint64_t bit_mask = { 0u, 0x80000000u };
    int i;
    uint64_t *bitboard = get_bitboard(color, type);

    for (i = 0; i < BOARD_SIZE; i++)
    {
        uint64_t result = bitwise_and(bitboard, &bit_mask);
        if (i % 8 == 0)
        {
            printf("\n");
        }
        if (result.low >= 1 || result.high >= 1)
        {
            printf("1");
        } else
        {
            printf("0");
        }
        bitwise_right_shift(&bit_mask, 1);
    }
    printf("\n");
}

void print_bit_in_bitboard(piece_color color, piece_type type, int idx)
{
    printf("bit at idx %d: %d", idx, is_bit_set(color, type, idx));
}

int get_idx_of_bit(piece_color color, piece_type type)
{
    uint64_t *bitboard = get_bitboard(color, type);
    int i, idx = -1;
    for (i = 0; i < 64; i++)
    {
        if (is_bit_set(color, type, i)) {idx = i; break;}
    }
    return idx;
}

int get_rank_of_bit(int idx)
{
    return idx / 8;
}

int get_file_of_bit(int idx)
{
    return 7 - (idx % 8);
}

int piece_present(piece_color color, int idx)
{
    int type;
    for (type = 0; type < 6; type++)
    {
        if (is_bit_set(color, type, idx) == true) {return type;}
    }
    return -1;
}

char *piece_idx_to_chess_notation(int idx)
{
    int rank = get_rank_of_bit(idx);
    int file = get_file_of_bit(idx);

    char *arr = malloc(3);
    arr[0] = file + 'a' - 1;
    arr[1] = rank + '0';
    arr[2] = '\0';

    return arr;
}

void clear_board()
{
    int i, color, type, board[64];
    for (color = WHITE; color <= BLACK; color++)
    {
        for (type = PAWN; type <= KING; type++)
        {
            for (i = 0; i < 64; i++)
            {
                board_array[i] = -1;
            }
        }
    }
}

void update_board()
{
    int i, color, type, board[64] = { 0 };
    for (color = WHITE; color <= BLACK; color++)
    {
        for (type = PAWN; type <= KING; type++)
        {
            for (i = 0; i < 64; i++)
            {
                int num_pieces = 6;
                if (is_bit_set(color, type, i)) { board_array[i] = type + (num_pieces * color); }

            }
        }
    }
}

void print_board() {
    int rank, file;
    char piece_symbols[] = { 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k' };

    printf("  +----+----+----+----+----+----+----+----+\n");
    for (rank = 7; rank >= 0; rank--)
    {
        printf("%d ", rank + 1);
        for (file = 7; file >= 0; file--)
        {
            int idx = rank * 8 + file;
            int piece = board_array[idx];
            char symbol = piece == -1 ? ' ' : piece_symbols[piece];
            printf("| %c  ", symbol);
        }
        printf("|\n  +----+----+----+----+----+----+----+----+\n");
    }
    printf("    a    b    c    d    e    f    g    h  \n");
}

void clear_bitboard(piece_color color, piece_type type)
{
    uint64_t *bitboard = get_bitboard(color, type);
    if (bitboard == NULL)
    {
        printf("Error: Attempted to clear a NULL bitboard at color %d, type %d\n", color, type);
        return;
    }
    bitboard->low = 0u;
    bitboard->high = 0u;
}

void update_bitboard(move *new_move, bool add_move)
{
    int square_start = new_move->square_start;
    int square_end = new_move->square_end;
    piece_color color = new_move->color;
    piece_type type = new_move->type;
    int castle = new_move->castle;
    int capture = new_move->capture;
    int pawn_promotion = new_move->pawn_promotion;
    int en_passant = new_move->en_passant;

    int en_passant_capture_square = color == WHITE ? square_end - 8 : square_end + 8;

    if (add_move)
    {
        if (capture != -1 && en_passant == 0) { clear_bit(!color, capture, square_end); }
        else if (capture != -1 && en_passant == 1) { clear_bit(!color, capture, en_passant_capture_square); }
        if (pawn_promotion != -1) { clear_bit(color, type, square_start); set_bit(color, pawn_promotion, square_end); }
        else { clear_bit(color, type, square_start); set_bit(color, type, square_end); }
    }
    else
    {
        if (pawn_promotion != -1) { clear_bit(color, pawn_promotion, square_end); set_bit(color, type, square_start); }
        else { clear_bit(color, type, square_end); set_bit(color, type, square_start); }
        if (capture != -1 && en_passant == 0) { set_bit(!color, capture, square_end); }
        else if (capture != -1 && en_passant == 1) { set_bit(!color, capture, en_passant_capture_square); }
    }

    if (castle != 0) { update_bitboard_castle(new_move, add_move); }

}

void update_bitboard_castle(move *new_move, bool add_move)
{
    int square_start = new_move->square_start;
    int square_end = new_move->square_end;
    piece_color color = new_move->color;
    int castle = new_move->castle;

    if (add_move && color == WHITE)
    {
        clear_bit(color, KING, square_start);
        clear_bit(color, ROOK, castle == 1 ? 0 : 7);
        set_bit(color, KING, castle == 1 ? 1 : 5);
        set_bit(color, ROOK, castle == 1 ? 2 : 4);
    }
    if (add_move && color == BLACK)
    {
        clear_bit(color, KING, square_start);
        clear_bit(color, ROOK, castle == 1 ? 56 : 63);
        set_bit(color, KING, castle == 1 ? 57 : 61);
        set_bit(color, ROOK, castle == 1 ? 58 : 60);
    }
    if (!add_move && color == WHITE)
    {
        set_bit(color, KING, square_start);
        set_bit(color, ROOK, castle == 1 ? 0 : 7);
        clear_bit(color, KING, castle == 1 ? 1 : 5);
        clear_bit(color, ROOK, castle == 1 ? 2 : 4);
    }
    if (!add_move && color == BLACK)
    {
        set_bit(color, KING, square_start);
        set_bit(color, ROOK, castle == 1 ? 56 : 63);
        clear_bit(color, KING, castle == 1 ? 57 : 61);
        clear_bit(color, ROOK, castle == 1 ? 58 : 60);
    }
}

void clear_all_bitboards()
{
    int i, j;
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 6; j++)
        {
            clear_bitboard(i, j);
        }
    }
}

void reset_all_bitboards()
{
    free(bitboards);
    init_boards();
}





