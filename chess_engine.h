#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

/*#include "memory_debug.h"*/

#define DEBUG_MODE 0

#define MAX_MOVES 1024
#define BOARD_SIZE 64


/* global structs and enums */
typedef enum { WHITE, BLACK } piece_color;
typedef enum { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING } piece_type;
typedef enum { PLAYING, STALEMATE, WHITE_CHECKMATED, BLACK_CHECKMATED, INSUFFICIENT_MATERIAL, THREEFOLD_REPETITION, FIFTY_MOVE_RULE } game_state;

typedef struct {
    unsigned int low;
    unsigned int high;
} uint64_t;

typedef struct
{
    uint64_t white_pawns;
    uint64_t white_knights;
    uint64_t white_bishops;
    uint64_t white_rooks;
    uint64_t white_queens;
    uint64_t white_kings;

    uint64_t black_pawns;
    uint64_t black_knights;
    uint64_t black_bishops;
    uint64_t black_rooks;
    uint64_t black_queens;
    uint64_t black_kings;
} Bitboard;

typedef struct
{
    int         square_start;
    int         square_end;
    piece_color color;
    piece_type  type;
    int         castle;         /* 0 = none, 1 = kingside, -1 = queenside */
    int         capture;        /* -1 = none, 0-5 = piece_type */
    int         pawn_promotion; /* -1 = none, 0-5 = piece_type */
    int         score;
    int         en_passant;     /* 0 = no, 1 = yes */
} move;


/* uint64_t */
extern uint64_t               bitwise_and(uint64_t *result, uint64_t *a);
extern uint64_t               bitwise_or(uint64_t *result, uint64_t *a);
extern void                   bitwise_left_shift(uint64_t *result, const short int shift);
extern void                   bitwise_right_shift(uint64_t *result, const short int shift);
extern void                   add(uint64_t *result, const uint64_t *a);
extern bool                   is_bit_set(piece_color color, piece_type type, int idx);
extern void                   set_bit(piece_color color, piece_type type, int idx);
extern void                   clear_bit(piece_color color, piece_type type, int idx);
extern int                    count_set_bits(piece_color color, piece_type type);


/* game.c */
extern move                 **moves_made_array_white;
extern move                 **moves_made_array_black;
extern int                    moves_made_count_white;
extern int                    moves_made_count_black;
extern piece_color            turn;
extern game_state             state;
extern void                   init_moves_made();
extern void                   moves_made_add(move *old_move);
extern void                   moves_made_remove(move *new_move);
extern void                   moves_made_remove_all(piece_color color);
extern void                   moves_made_print_all(piece_color color);
extern void                   moves_made_print_last(piece_color color);
extern bool                   is_move_safe(move *new_move);
extern bool                   has_piece_moved(int square_start, piece_color color, piece_type type);
extern int                    get_move_made_idx(int square_start, int square_end, piece_color color, int pawn_promotion);
extern void                   check_game_state();
extern void                   game_loop();
extern move*                  get_input();


/* board.c */
extern Bitboard              *bitboards;
extern int                   *board_array;
extern void                   init_boards();
extern uint64_t              *get_bitboard(piece_color color, piece_type type);
extern void                   print_bitboard(piece_color color, piece_type type);
extern void                   print_bit_in_bitboard(piece_color color, piece_type type, int idx);
extern int                    get_idx_of_bit(piece_color color, piece_type type);
extern int                    get_rank_of_bit(int idx);
extern int                    get_file_of_bit(int idx);
extern int                    piece_present(piece_color color, int idx);
extern char                  *piece_idx_to_chess_notation(int idx);
extern void                   clear_board();
extern void                   update_board();
extern void                   print_board();
extern void                   clear_bitboard(piece_color color, piece_type type);
extern void                   update_bitboard(move *new_move, bool add_move);
extern void                   update_bitboard_castle(move *new_move, bool add_move);
extern void                   clear_all_bitboards();
extern void                   reset_all_bitboards();


/* move_generation.c */
extern move                 **moves_possible_array_white;
extern move                 **moves_possible_array_black;
extern int                    moves_possible_count_white;
extern int                    moves_possible_count_black;
extern const int              pawn_moves[2][4][2];
extern const int              knight_moves[8][2];
extern const int              diagonal_moves[4][2];
extern const int              straight_moves[4][2];
extern void                   init_moves_possible();
extern void                   moves_possible_add(int square_start, int square_end, piece_color color, piece_type type, int castle, int capture, int pawn_promotion, int score, int en_passant);
extern void                   moves_possible_remove(int square_start, int square_end, piece_color color, piece_type type, int castle, int capture, int pawn_promotion);
extern void                   moves_possible_remove_all(piece_color color);
extern void                   moves_possible_print_all(piece_color color);
extern void                   moves_possible_print_all_details(piece_color color);
extern void                   moves_possible_print_all_safe(piece_color color);
extern bool                   is_duplicate_move(int square_start, int square_end, piece_color color, int pawn_promotion);
extern bool                   is_path_clear(int* squares, piece_color color, int size);
extern bool                   is_path_safe(int* squares, piece_color color, int size);
extern bool                   is_in_check(piece_color color);
extern bool                   is_rank_file_index_valid(int start_idx, const int direction[], const int step);
extern bool                   is_square_targeted(int idx, piece_color color);
extern int                    get_target_rank(int starting_rank, const int direction[], int step);
extern int                    get_target_file(int starting_file, const int direction[], int step);
extern int                    get_target_idx(int starting_idx, const int direction[], int step);
extern int                    get_move_possible_idx(int square_start, int square_end, piece_color color, int pawn_promotion);
extern void                   generate_directional_moves(const int directions[][2], const int dir_count, piece_color color, piece_type type, bool single_step);
extern void                   generate_castling_moves(piece_color color);
extern void                   generate_pawn_moves(piece_color color);
extern void                   generate_knight_moves(piece_color color);
extern void                   generate_bishop_moves(piece_color color);
extern void                   generate_rook_moves(piece_color color);
extern void                   generate_queen_moves(piece_color color);
extern void                   generate_king_moves(piece_color color);
extern void                   moves_possible_generate(piece_color color);


/* evaluation.c */
extern move                 **moves_candidate_array_white;
extern move                 **moves_candidate_array_black;
extern int                    moves_candidate_count_white;
extern int                    moves_candidate_count_black;
extern void                   init_moves_candidate();
extern void                   moves_candidate_add(move *old_move);
extern void                   moves_candidate_remove(move *new_move);
extern void                   moves_candidate_remove_all(piece_color color);
extern void                   moves_candidate_print_all(piece_color color);
extern void                   moves_candidate_generate(piece_color color);
extern move                  *moves_candidate_get_best(piece_color color);
extern int                    get_move_candidate_idx(move *new_move);
extern int                    evaluate_board();
extern int                    evaluate_board_material();
extern int                    evaluate_move(move *new_move);
extern int                    evaluate_move_material(move *new_move);


/* play.c */
extern void                   cpu_play_cpu();



/* tests */
extern void                   print_move(move *new_move);
extern void                   test_evaluation();


#endif /* CHESS_ENGINE_H */
