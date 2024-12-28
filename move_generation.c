#include "chess_engine.h"

#define BOARD_RANKS 8
#define BOARD_FILES 8

#define MAX_RANK (BOARD_RANKS - 1)
#define MAX_FILE (BOARD_FILES - 1)
#define MIN_RANK (BOARD_RANKS - BOARD_RANKS)
#define MIN_FILE (BOARD_FILES - BOARD_FILES)

move **moves_possible_array_white = NULL;
move **moves_possible_array_black = NULL;
int moves_possible_count_white = 0;
int moves_possible_count_black = 0;

const int pawn_moves[2][4][2] = {
    { { 2, 0}, { 1, 0}, { 1,  1}, { 1, -1} }, /* white pawns */
    { {-2, 0}, {-1, 0}, {-1, -1}, {-1,  1} }  /* black pawns */
};

const int knight_moves[8][2] = {
    { 2,  1}, { 2, -1}, {-2,  1}, {-2, -1},
    { 1,  2}, { 1, -2}, {-1,  2}, {-1, -2}
};

const int diagonal_moves[4][2] = { { 1,  1}, {-1,  1}, { 1, -1}, {-1, -1} };
const int straight_moves[4][2] = { { 1,  0}, { 0,  1}, {-1,  0}, { 0, -1} };

void init_moves_possible()
{
    int i;
    moves_possible_array_white = malloc(sizeof(move *) * MAX_MOVES);
    moves_possible_array_black = malloc(sizeof(move *) * MAX_MOVES);

    if (moves_possible_array_white == NULL || moves_possible_array_black == NULL)
    {
        printf("Malloc failed for moves made array. \n");
        exit(1);
    }

    for (i = 0; i < MAX_MOVES; i++)
    {
        moves_possible_array_white[i] = NULL;
        moves_possible_array_black[i] = NULL;
    }

    if (DEBUG_MODE) {
        printf("---[INIT MOVES POSSIBLE]---\n"
               "moves_possible_array_white: %p\n"
               "moves_possible_array_black: %p\n",
               moves_possible_array_white,
               moves_possible_array_black);
    }
}

void moves_possible_add(int square_start, int square_end, piece_color color, piece_type type, int castle, int capture, int pawn_promotion, int score, int en_passant)
{
    if (is_duplicate_move(square_start, square_end, color, pawn_promotion)) {return;}

    int *moves_possible_count;
    move **moves_possible_array;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? &moves_possible_count_white : &moves_possible_count_black;

    move *new_move = malloc(sizeof(move));
    if (new_move == NULL) {
        printf("Memory allocation failed for moves possible add. \n");
        exit(1);
    }

    new_move->square_start = square_start;
    new_move->square_end = square_end;
    new_move->color = color;
    new_move->type = type;
    new_move->castle = castle;
    new_move->capture = capture;
    new_move->pawn_promotion = pawn_promotion;
    new_move->score = score;
    new_move->en_passant = en_passant;

    moves_possible_array[*moves_possible_count] = new_move;
    *moves_possible_count = *moves_possible_count + 1;
}

void moves_possible_remove(int square_start, int square_end, piece_color color, piece_type type, int castle, int capture, int pawn_promotion)
{
    int i, *moves_possible_count;
    move **moves_possible_array;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? &moves_possible_count_white : &moves_possible_count_black;
    int move_idx = get_move_possible_idx(square_start, square_end, color, pawn_promotion);

    if (DEBUG_MODE) {
        printf("---[MOVES POSSIBLE REMOVE]---\n"
               "square_start: %d\n"
               "square_end: %d\n"
               "color: %d\n"
               "type: %d\n"
               "castle: %d\n"
               "capture: %d\n"
               "move_idx: %d\n"
               "moves_possible_count_white: %d\n"
               "moves_possible_count_black: %d\n"
               "moves_possible_array address: %p\n",
               square_start,
               square_end,
               color,
               type,
               castle,
               capture,
               move_idx,
               moves_possible_count_white,
               moves_possible_count_black,
               moves_possible_array);
    }

    if (moves_possible_array[move_idx] != NULL)
    {
        free(moves_possible_array[move_idx]);
        moves_possible_array[move_idx]= NULL;
    }

    for (i = move_idx; i < *moves_possible_count; i++)
    {
        moves_possible_array[i] = moves_possible_array[i + 1];
    }

    moves_possible_array[*moves_possible_count - 1] = NULL;
    *moves_possible_count = *moves_possible_count - 1;
}

void moves_possible_remove_all(piece_color color)
{
    int i, *moves_possible_count;
    move **moves_possible_array;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? &moves_possible_count_white : &moves_possible_count_black;

    if (DEBUG_MODE) {
        printf("---[MOVES POSSIBLE REMOVE ALL]---\n"
               "color: %d\n"
               "moves_possible_count_white: %d\n"
               "moves_possible_count_black: %d\n"
               "moves_possible_array address: %p\n",
               color,
               moves_possible_count_white,
               moves_possible_count_black,
               moves_possible_array);
    }

    for (i = 0; i < *moves_possible_count; i++)
    {
        if (moves_possible_array[i] != NULL)
        {
            free(moves_possible_array[i]);
            moves_possible_array[i] = NULL;
        }
    }
    *moves_possible_count = 0;
}

void moves_possible_print_all(piece_color color) {
    int i, moves_possible_count;
    move **moves_possible_array;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? moves_possible_count_white : moves_possible_count_black;

    if (DEBUG_MODE) {
        printf("---[MOVES POSSIBLE PRINT ALL]---\n"
               "color: %d\n"
               "moves_possible_count_white: %d\n"
               "moves_possible_count_black: %d\n"
               "moves_possible_array address: %p\n",
               color,
               moves_possible_count_white,
               moves_possible_count_black,
               moves_possible_array);
    }

    for (i = 0; i < moves_possible_count; i++) {
        move *new_move = moves_possible_array[i];

        int capture = new_move->capture;
        int pawn_promotion = new_move->pawn_promotion;
        char piece = " NBRQK"[new_move->type];
        char pp_piece = " NBRQK"[new_move->pawn_promotion];

        int start_rank = get_rank_of_bit(new_move->square_start) + 1;
        char start_file = get_file_of_bit(new_move->square_start) + 'a';
        int end_rank = get_rank_of_bit(new_move->square_end) + 1;
        char end_file = get_file_of_bit(new_move->square_end) + 'a';

        printf("---[MOVE POSSIBLE INFO]: start: %d, "
        "end: %d, "
        "color: %d, "
        "type: %d, "
        "castle: %d, "
        "capture: %d, "
        "pp: %d, "
        "score: %d, "
        "enpas: %d\n",
        new_move->square_start,
        new_move->square_end,
        new_move->color,
        new_move->type,
        new_move->castle,
        new_move->capture,
        new_move->pawn_promotion,
        new_move->score,
        new_move->en_passant);
    }
}

void moves_possible_print_all_details(piece_color color)
{
    int i, moves_possible_count;
    move **moves_possible_array;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? moves_possible_count_white : moves_possible_count_black;

    printf("---[MOVES POSSIBLE PRINT ALL DETAILS %s]---\n", color == WHITE ? "WHITE" : "BLACK");

    for (i = 0; i < moves_possible_count; i++) {
        move *new_move = moves_possible_array[i];

        printf("Move idx %d:\n"
               "new_move->square_start: %d,\n"
               "new_move->square_end: %d,\n"
               "new_move->color: %d,\n"
               "new_move->type: %d,\n"
               "new_move->castle: %d,\n"
               "new_move->capture: %d,\n"
               "new_move->pawn_promotion: %d,\n",
               i,
               new_move->square_start,
               new_move->square_end,
               new_move->color,
               new_move->type,
               new_move->castle,
               new_move->capture,
               new_move->pawn_promotion);
    }
}

void moves_possible_print_all_safe(piece_color color)
{
    int i, moves_possible_count;
    move **moves_possible_array;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? moves_possible_count_white : moves_possible_count_black;

    for (i = 0; i < moves_possible_count; i++)
    {
        move *new_move = moves_possible_array[i];

        if (!is_move_safe(new_move)) { continue; }

        int capture = new_move->capture;
        int pawn_promotion = new_move->pawn_promotion;
        char piece = " NBRQK"[new_move->type];
        char pp_piece = " NBRQK"[new_move->pawn_promotion];

        int start_rank = get_rank_of_bit(new_move->square_start) + 1;
        char start_file = get_file_of_bit(new_move->square_start) + 'a';
        int end_rank = get_rank_of_bit(new_move->square_end) + 1;
        char end_file = get_file_of_bit(new_move->square_end) + 'a';

        printf("----------[SAFE MOVE BY %s]-------- %d: %c%c%d%c%c%d%c%c\n",
               color == WHITE?"WHITE":"BLACK",
               i,
               piece,
               start_file, start_rank,
               capture != -1 ? 'x' : ' ',
               end_file, end_rank,
               pawn_promotion != -1 ? '=' : ' ',
               pp_piece);
    }
}

bool is_square_targeted(int idx, piece_color color)
{
    /* is the square targeted by a piece of this color. pass in opponent's color */
    int i, moves_possible_count;
    move **moves_possible_array;
    bool flag = false;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? moves_possible_count_white : moves_possible_count_black;

    if (moves_possible_count == 0) {return false;}

    for (i = 0; i < moves_possible_count; i++)
    {
        if (moves_possible_array[i]->color != color) {continue;}
        if (moves_possible_array[i]->square_end == idx) {flag = true;}

        if (DEBUG_MODE)
        {
            printf("---[IS SQ TARGETED]---\n"
                   "i: %d\n"
                   "idx: %d\n"
                   "moves_possible_count: %d\n"
                   "moves_possible_array[i] address: %p\n"
                   "flag: %d\n"
                   "color: %d\n"
                   "moves_possible_array[i]->color==color: %d\n"
                   "moves_possible_array[i]->square_end==idx: %d\n",
                   i,
                   idx,
                   moves_possible_count,
                   moves_possible_array[i],
                   flag ? 1 : 0,
                   color,
                   moves_possible_array[i]->color==color?1:0,
                   moves_possible_array[i]->square_end==idx?1:0);
        }
    }

    return flag;
}

bool is_path_clear(int* squares, piece_color color, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (piece_present(color, squares[i]) != -1 || piece_present(!color, squares[i]) != -1) {
            if (DEBUG_MODE) {
                printf("---[IS PATH CLEAR]---\n"
                       "i: %d\n"
                       "square: %d\n"
                       "piece_present(color, square): %d\n"
                       "piece_present(!color, square): %d\n"
                       "color: %d\n",
                       i,
                       squares[i],
                       piece_present(color, squares[i]),
                       piece_present(!color, squares[i]),
                       color);
            }
            return false;
        }
    }
    return true;
}

bool is_path_safe(int* squares, piece_color color, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (is_square_targeted(squares[i], color)) {
            if (DEBUG_MODE) {
                printf("---[IS PATH SAFE]---\n"
                       "i: %d\n"
                       "square: %d\n"
                       "is_square_targeted(square, color): %d\n"
                       "color: %d\n",
                       i,
                       squares[i],
                       is_square_targeted(squares[i], color) ? 1 : 0,
                       color);
            }
            return false;
        }
    }
    return true;
}

bool is_rank_file_index_valid(int start_idx, const int direction[], const int step)
{
    bool result;

    int start_rank = get_rank_of_bit(start_idx);
    int start_file = get_file_of_bit(start_idx);
    int target_rank = get_target_rank(start_rank, direction, step);
    int target_file = get_target_file(start_file, direction, step);
    int target_idx = get_target_idx(start_idx, direction, step);

    int rank_offset = direction[0];
    int file_offset = direction[1];

    bool res0 = target_rank < MIN_RANK;
    bool res1 = target_rank > MAX_RANK;
    bool res2 = target_file < MIN_FILE;
    bool res3 = target_file > MAX_FILE;
    bool res4 = target_idx >= BOARD_SIZE;
    bool res5 = target_idx < 0;
    bool res6 = !(direction[0] >= 0 ? MAX_RANK - start_rank >= direction[0] : start_rank >= (direction[0] * -1));
    bool res7 = !(direction[1] >= 0 ? MAX_FILE - start_file >= direction[1] : start_file >= (direction[1] * -1));

    if (res0 || res1 || res2 || res3 || res4 || res5 || res6 || res7)
    {
        result = false;
    } else {result = true;}

    if (DEBUG_MODE)
    {
        printf("---[IS RFI VALID]---\n"
               "start_rank: %d\n"
               "start_file: %d\n"
               "target_rank: %d\n"
               "target_file: %d\n"
               "direction: %d, %d\n"
               "step: %d\n"
               "target_idx: %d\n"
               "rank_offset: %d\n"
               "file_offset: %d\n"
               "res0: %d,\n"
               "res1: %d,\n"
               "res2: %d,\n"
               "res3: %d,\n"
               "res4: %d,\n"
               "res5: %d,\n"
               "res6: %d,\n"
               "res7: %d\n",
               start_rank,
               start_file,
               target_rank,
               target_file,
               direction[0],
               direction[1],
               step,
               target_idx,
               rank_offset,
               file_offset,
               res0,
               res1,
               res2,
               res3,
               res4,
               res5,
               res6,
               res7);
    }

    return result;
}

bool is_in_check(piece_color color)
{
    int king_idx = get_idx_of_bit(color, KING);
    bool targeted = is_square_targeted(king_idx, !color);

    if (DEBUG_MODE) {
        printf("---[IS IN CHECK]---\n"
               "color: %d\n"
               "king_idx: %d\n"
               "targeted: %d\n",
               color,
               king_idx,
               targeted ? 1 : 0);
    }

    return targeted;
}

bool is_duplicate_move(int square_start, int square_end, piece_color color, int pawn_promotion)
{
    int i, moves_possible_count;
    move **moves_possible_array;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? moves_possible_count_white : moves_possible_count_black;

    if (DEBUG_MODE) {
        printf("---[IS DUPLICATE MOVE]---\n"
               "square_start: %d\n"
               "square_end: %d\n"
               "color: %d\n"
               "moves_possible_count: %d\n"
               "moves_possible_array: %p\n",
               square_start,
               square_end,
               color,
               moves_possible_count,
               moves_possible_array);
    }

    for (i = 0; i < moves_possible_count; i++)
    {
        if (moves_possible_array[i]->square_start == square_start
            && moves_possible_array[i]->square_end == square_end &&
            moves_possible_array[i]->pawn_promotion == pawn_promotion)
        {
            return true;
        }
    }
    return false;
}

int get_target_rank(int starting_rank, const int direction[], int step)
{
    int target_rank = starting_rank + direction[0] * step;

    if (DEBUG_MODE)
    {
        printf("---[GET TARGET RANK]---\n"
               "starting_rank: %d\n"
               "direction[0]: %d\n"
               "direction[1]: %d\n"
               "step: %d\n"
               "target_rank: %d\n",
               starting_rank,
               direction[0],
               direction[1],
               step,
               target_rank);
    }

    return target_rank;
}

int get_target_file(int starting_file, const int direction[], int step)
{
    int target_file = starting_file + direction[1] * step;

    if (DEBUG_MODE)
    {
        printf("---[GET TARGET FILE]---\n"
               "starting_file: %d\n"
               "direction[0]: %d\n"
               "direction[1]: %d\n"
               "step: %d\n"
               "target_file: %d\n",
               starting_file,
               direction[0],
               direction[1],
               step,
               target_file);
    }

    return target_file;
}

int get_target_idx(int starting_idx, const int direction[], int step) {
    int start_rank = get_rank_of_bit(starting_idx);
    int start_file = get_file_of_bit(starting_idx);
    int target_rank = get_target_rank(start_rank, direction, step);
    int target_file = get_target_file(start_file, direction, step);

    int rank_offset = direction[0];
    int file_offset = direction[1];

    int target_idx = starting_idx + (rank_offset * BOARD_RANKS * step) - (file_offset * step);

    if (DEBUG_MODE)
    {
        printf("---[GET TARGET IDX]---\n"
               "start_rank: %d\n"
               "start_file: %d\n"
               "target_rank: %d\n"
               "target_file: %d\n"
               "rank_offset: %d\n"
               "file_offset: %d\n"
               "target_idx: %d\n"
               "direction[0]: %d\n"
               "direction[1]: %d\n"
               "step: %d\n"
               "starting_idx: %d\n",
               start_rank,
               start_file,
               target_rank,
               target_file,
               rank_offset,
               file_offset,
               target_idx,
               direction[0],
               direction[1],
               step,
               starting_idx);
    }

    return target_idx;
}

int get_move_possible_idx(int square_start, int square_end, piece_color color, int pawn_promotion)
{
    int i, moves_possible_count;
    move **moves_possible_array;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? moves_possible_count_white : moves_possible_count_black;

    if (DEBUG_MODE) {
        printf("---[GET MOVE POSSIBLE IDX]---\n"
               "square_start: %d\n"
               "square_end: %d\n"
               "color: %d\n"
               "moves_possible_count: %d\n"
               "moves_possible_array: %p\n",
               square_start,
               square_end,
               color,
               moves_possible_count,
               moves_possible_array);
    }

    for (i = 0; i < moves_possible_count; i++)
        if (moves_possible_array[i]->square_start == square_start
            && moves_possible_array[i]->square_end == square_end
            && moves_possible_array[i]->pawn_promotion == pawn_promotion)
        {
            return i;
        }
    return -1;
}

void generate_directional_moves(const int directions[][2], const int dir_count, piece_color color, piece_type type, bool single_step) {
    int i, d, step;
    int step_count = single_step ? 1 : BOARD_RANKS;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (!is_bit_set(color, type, i)) {continue;}
        for (d = 0; d < dir_count; d++)
        {
            for (step = 1; step <= step_count; step++)
            {
                int target_idx = get_target_idx(i, directions[d], step);
                int friendly_present = piece_present(color, target_idx);
                int enemy_present = piece_present(!color, target_idx);
                bool rfi_valid = is_rank_file_index_valid(i, directions[d], step);

                if (friendly_present != -1) {break;}
                if (!rfi_valid) {continue;}

                moves_possible_add(i, target_idx, color, type, 0, enemy_present, -1, 0, 0);

                if (DEBUG_MODE)
                {
                    printf("---[MOVE ADD]---\n"
                           "starting idx %d\n"
                       "target idx %d\n"
                       "direction %d %d\n"
                       "dir count %d\n"
                       "color %d\n"
                       "piece type %d\n"
                       "friendly present %d\n"
                       "enemy present %d\n",
                       i,
                       target_idx,
                       directions[d][0],
                       directions[d][1],
                       dir_count,
                       color,
                       type,
                       friendly_present,
                       enemy_present);

                }

                if (enemy_present != -1 || step_count == 1) {break;}
            }
        }
    }
}

void generate_castling_moves(piece_color color)
{
    bool white_kingside_castle_flag, white_queenside_castle_flag, black_kingside_castle_flag, black_queenside_castle_flag;

    int white_kingside_squares[2] = {1, 2};
    int white_queenside_squares[3] = {4, 5, 6};
    int black_kingside_squares[2] = {57, 58};
    int black_queenside_squares[3] = {60, 61, 62};

    bool white_king_in_check = is_square_targeted(3, BLACK);
    bool black_king_in_check = is_square_targeted(59, WHITE);

    bool white_kingside_rook_present = is_bit_set(WHITE, ROOK, 0);
    bool white_queenside_rook_present = is_bit_set(WHITE, ROOK, 7);

    bool black_kingside_rook_present = is_bit_set(BLACK, ROOK, 56);
    bool black_queenside_rook_present = is_bit_set(BLACK, ROOK, 63);

    white_kingside_castle_flag = is_path_clear(white_kingside_squares, WHITE, 2) &&
        is_path_safe(white_kingside_squares, BLACK, 2) &&
            !has_piece_moved(0, WHITE, ROOK) &&
                !has_piece_moved(3, WHITE, KING) &&
                    !white_king_in_check &&
                        white_kingside_rook_present &&
                        color == WHITE;

    white_queenside_castle_flag = is_path_clear(white_queenside_squares, WHITE, 3) &&
    is_path_safe(white_queenside_squares, BLACK, 3) &&
        !has_piece_moved(7, WHITE, ROOK) &&
            !has_piece_moved(3, WHITE, KING) &&
                !white_king_in_check &&
                    white_queenside_rook_present &&
                        color == WHITE;

    black_kingside_castle_flag = is_path_clear(black_kingside_squares, BLACK, 2) &&
    is_path_safe(black_kingside_squares, WHITE, 2) &&
        !has_piece_moved(56, BLACK, ROOK) &&
            !has_piece_moved(59, BLACK, KING) &&
                !black_king_in_check &&
                    black_kingside_rook_present &&
                        color == BLACK;

    black_queenside_castle_flag = is_path_clear(black_queenside_squares, BLACK, 3) &&
    is_path_safe(black_queenside_squares, WHITE, 3) &&
        !has_piece_moved(63, BLACK, ROOK) &&
            !has_piece_moved(59, BLACK, KING) &&
                !black_king_in_check &&
                    black_queenside_rook_present &&
                        color == BLACK;

    if (white_kingside_castle_flag && color == WHITE) {moves_possible_add(3, 1, WHITE, KING, 1, 0, -1, 0, 0);}
    if (white_queenside_castle_flag && color == WHITE) {moves_possible_add(3, 5, WHITE, KING, -1, 0, -1, 0, 0);}
    if (black_kingside_castle_flag && color == BLACK) {moves_possible_add(59, 57, BLACK, KING, 1, 0, -1, 0, 0);}
    if (black_queenside_castle_flag && color == BLACK) {moves_possible_add(59, 61, BLACK, KING, -1, 0, -1, 0, 0);}

    if (DEBUG_MODE)
    {
        printf("---[GENERATE CASTLING MOVES]---\n"
               "wk_clear: %d, \n"
               "wq_clear: %d, \n"
               "wk_safe: %d, \n"
               "wq_safe: %d, \n"
               "wkr_moved: %d, \n"
               "wqr_moved: %d, \n"
               "wk_moved: %d, \n"
               "wk_check: %d, \n"
               "wkr_pres: %d, \n"
               "wqr_pres: %d, \n"
               "...wkcf: %d, \n"
               "wqcf: %d \n",
               is_path_clear(white_kingside_squares, WHITE, 2),
               is_path_clear(white_queenside_squares, WHITE, 3),
               is_path_safe(white_kingside_squares, BLACK, 2),
               is_path_safe(white_queenside_squares, BLACK, 3),
               has_piece_moved(0, WHITE, ROOK),
               has_piece_moved(7, WHITE, ROOK),
               has_piece_moved(3, WHITE, KING),
               white_king_in_check,
               white_kingside_rook_present,
               white_queenside_rook_present, white_kingside_castle_flag,  white_queenside_castle_flag
               );

        printf("bk_clear: %d, \n"
               "bq_clear: %d, \n"
               "bk_safe: %d, \n"
               "bq_safe: %d, \n"
               "bkr_moved: %d, \n"
               "bqr_moved: %d, \n"
               "bk_moved: %d, \n"
               "bk_check: %d, \n"
               "bkr_pres: %d, \n"
               "bqr_pres: %d, \n"
               "...bkcf: %d, \n"
               "bqcf: %d \n",
               is_path_clear(black_kingside_squares, BLACK, 2),
               is_path_clear(black_queenside_squares, BLACK, 3),
               is_path_safe(black_kingside_squares, WHITE, 2),
               is_path_safe(black_queenside_squares, WHITE, 3),
               has_piece_moved(56, BLACK, ROOK),
               has_piece_moved(63, BLACK, ROOK),
               has_piece_moved(59, BLACK, KING),
               black_king_in_check,
               black_kingside_rook_present,
               black_queenside_rook_present, black_kingside_castle_flag, black_queenside_castle_flag
               );
    }
}

void generate_pawn_moves(piece_color color) {

    int i, d, piece, adv_one_idx, adv_two_idx, left_cap_idx, right_cap_idx,
    start_rank, start_file, step = 1, moves_made_count_opp, capture, en_pas_idx, *moves_possible_count;
    bool able_to_promote, en_pas_flag;
    move **moves_made_array_opp, **moves_possible_array;
    moves_made_array_opp = color == WHITE ? moves_made_array_black : moves_made_array_white;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? &moves_possible_count_white : &moves_possible_count_black;
    moves_made_count_opp = color == WHITE ? moves_made_count_black : moves_made_count_white;
    piece_type type = PAWN;
    uint64_t *bitboard = get_bitboard(color, type);
    const int (*directions)[2] = pawn_moves[color];
    const int dir_count = 4;

    move *last_opp_color_move;
    static move empty_move = {-1, -1, -1, -1, 0, -1, -1};

    if (moves_made_count_opp < 1) {last_opp_color_move = &empty_move;}
    else {last_opp_color_move = moves_made_array_opp[moves_made_count_opp - 1];}

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (!is_bit_set(color, type, i)) {continue;}

        start_rank = get_rank_of_bit(i);
        start_file = get_file_of_bit(i);
        adv_one_idx = color == WHITE ? i + BOARD_RANKS : i - BOARD_RANKS;
        adv_two_idx = color == WHITE ? i + BOARD_RANKS * 2 : i - BOARD_RANKS * 2;
        left_cap_idx = color == WHITE ? i + BOARD_RANKS + 1 : i - BOARD_RANKS - 1;
        right_cap_idx = color == WHITE ? i + BOARD_RANKS - 1 : i - BOARD_RANKS + 1;

        en_pas_flag = false;
        en_pas_idx = -1;

        bool opp_pawn_adv_two_on_last_move = color == BLACK ?
            last_opp_color_move->square_end - last_opp_color_move->square_start == BOARD_RANKS * 2
          : last_opp_color_move->square_start - last_opp_color_move->square_end == BOARD_RANKS * 2;


        if ( opp_pawn_adv_two_on_last_move
            && (get_file_of_bit(last_opp_color_move->square_end) == start_file + 1 || get_file_of_bit(last_opp_color_move->square_end) == start_file - 1)
            && (color == WHITE ? get_rank_of_bit(i) == 4 : get_rank_of_bit(i) == 3)
            && last_opp_color_move->type == PAWN)
        {
            en_pas_flag = true;
            en_pas_idx = color == WHITE ? last_opp_color_move->square_end + BOARD_RANKS : last_opp_color_move->square_end - BOARD_RANKS;
        }

        bool adv_one_flag = (piece_present(color, adv_one_idx) == -1)
            && (piece_present(!color, adv_one_idx) == -1);

        bool adv_two_flag = adv_one_flag
            && start_rank == (color == WHITE ? MIN_RANK + 1 : MAX_RANK - 1)
            && (piece_present(color, adv_two_idx) == -1)
            && (piece_present(!color, adv_two_idx) == -1);

        int left_capture = piece_present(!color, left_cap_idx);
        int right_capture = piece_present(!color, right_cap_idx);

        if (en_pas_flag)
        {
            moves_possible_add(i, en_pas_idx, color, type, 0, PAWN, -1, 0, 1);
            en_pas_flag = false;
            en_pas_idx = -1;
        }


        for (d = 0; d < dir_count; d++)
        {
            int target_idx = get_target_idx(i, directions[d], step);
            bool valid = is_rank_file_index_valid(i, directions[d], step);

            int enemy_present = piece_present(!color, target_idx);

            bool left_cap_flag = left_capture != -1 && d == 3;
            bool right_cap_flag = right_capture != -1 && d == 2;

            bool should_add_move = valid &&
                ((d == 0 && adv_two_flag) ||
                (d == 1 && adv_one_flag) ||
                (left_cap_flag) ||
                (right_cap_flag));

            int target_rank = get_rank_of_bit(target_idx);
            able_to_promote = false;
            if ((color == WHITE && target_rank == 7) || (color == BLACK && target_rank == 0))
            {
                able_to_promote = true;
            }

            if (DEBUG_MODE)
            {
                printf("----------[PAWN MOVE]----------\n"
                       "current_idx: %d\n"
                       "target_idx: %d\n"
                       "current_dir: %d\n"
                       "adv_one_idx: %d\n"
                       "adv_two_idx: %d\n"
                       "left_cap_idx: %d\n"
                       "right_cap_idx: %d\n"
                       "start_rank: %d\n"
                       "start_file: %d\n"
                       "adv_one_flag: %d\n"
                       "adv_two_flag: %d\n"
                       "left_capture: %d\n"
                       "right_capture: %d\n"
                       "en_pas_flag: %d\n"
                       "en_pas_idx: %d\n"
                       "piece_color: %d\n"
                       "piece_type: %d\n"
                       "valid: %d\n"
                       "should_add_move: %d\n"
                       "captured_piece: %d\n",
                       i,
                       target_idx,
                       d,
                       adv_one_idx,
                       adv_two_idx,
                       left_cap_idx,
                       right_cap_idx,
                       start_rank,
                       start_file,
                       adv_one_flag,
                       adv_two_flag,
                       left_capture,
                       right_capture,
                       en_pas_flag,
                       en_pas_idx,
                       color,
                       type,
                       valid ? 1 : 0,
                       should_add_move ? 1 : 0,
                       capture
                );
            }

            if (d == 2 && right_capture != -1) {capture = right_capture;}
            else if (d == 3 && left_capture != -1) {capture = left_capture;}
            else {capture = -1;}

            if (should_add_move && able_to_promote)
            {
                for (piece = 1; piece < KING; piece++)
                {
                    moves_possible_add(i, target_idx, color, type, 0, capture, piece, 0, 0);
                }
            }
            else if (should_add_move)
            {
                moves_possible_add(i, target_idx, color, type, 0, capture, -1, 0, 0);
            }
        }
    }
}

void generate_knight_moves(piece_color color) {
    int dir_count = 8;
    generate_directional_moves(knight_moves, dir_count, color, KNIGHT, true);
}

void generate_bishop_moves(piece_color color) {
    int dir_count = 4;
    generate_directional_moves(diagonal_moves, dir_count, color, BISHOP, false);
}

void generate_rook_moves(piece_color color) {
    int dir_count = 4;
    generate_directional_moves(straight_moves, dir_count, color, ROOK, false);
}

void generate_queen_moves(piece_color color) {
    int dir_count = 4;
    generate_directional_moves(diagonal_moves, dir_count, color, QUEEN, false);
    generate_directional_moves(straight_moves, dir_count, color, QUEEN, false);
}

void generate_king_moves(piece_color color) {
    int dir_count = 4;
    generate_directional_moves(diagonal_moves, dir_count, color, KING, true);
    generate_directional_moves(straight_moves, dir_count, color, KING, true);
}

void moves_possible_generate(piece_color color)
{
    moves_possible_remove_all(color);

    generate_pawn_moves(color);
    generate_knight_moves(color);
    generate_bishop_moves(color);
    generate_rook_moves(color);
    generate_queen_moves(color);
    generate_king_moves(color);
    generate_castling_moves(color);
}







