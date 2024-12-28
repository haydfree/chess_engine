#include "chess_engine.h"

move **moves_candidate_array_white = NULL;
move **moves_candidate_array_black = NULL;
int moves_candidate_count_white = 0;
int moves_candidate_count_black = 0;

void init_moves_candidate()
{
    int i;
    moves_candidate_array_white = malloc(sizeof(move *) * MAX_MOVES);
    moves_candidate_array_black = malloc(sizeof(move *) * MAX_MOVES);

    if (moves_candidate_array_white == NULL || moves_candidate_array_black == NULL)
    {
        printf("Malloc failed for moves candidate array. \n");
        exit(1);
    }

    for (i = 0; i < MAX_MOVES; i++)
    {
        moves_candidate_array_white[i] = NULL;
        moves_candidate_array_black[i] = NULL;
    }

    if (DEBUG_MODE) {
        printf("---[INIT MOVES CANDIDATE]---\n"
               "moves_candidate_array_white: %p\n"
               "moves_candidate_array_black: %p\n",
               moves_candidate_array_white,
               moves_candidate_array_black);
    }
}

void moves_candidate_add(move *old_move)
{
    int *moves_candidate_count;
    move **moves_candidate_array;
    moves_candidate_array = old_move->color == WHITE ? moves_candidate_array_white : moves_candidate_array_black;
    moves_candidate_count = old_move->color == WHITE ? &moves_candidate_count_white : &moves_candidate_count_black;

    move *new_move = malloc(sizeof(move));
    if (new_move == NULL) {
        printf("Memory allocation failed for moves candidate add.\n");
        exit(1);
    }

    new_move->square_start = old_move->square_start;
    new_move->square_end = old_move->square_end;
    new_move->color = old_move->color;
    new_move->type = old_move->type;
    new_move->castle = old_move->castle;
    new_move->capture = old_move->capture;
    new_move->pawn_promotion = old_move->pawn_promotion;
    new_move->score = old_move->score;
    new_move->en_passant = old_move->en_passant;

    moves_candidate_array[*moves_candidate_count] = new_move;
    *moves_candidate_count = *moves_candidate_count + 1;
}

void moves_candidate_remove(move *new_move)
{
    int square_start = new_move->square_start;
    int square_end = new_move->square_end;
    piece_color color = new_move->color;
    piece_type type = new_move->type;
    int castle = new_move->castle;
    int capture = new_move->capture;
    int pawn_promotion = new_move->pawn_promotion;
    int score = new_move->score;

    int i, idx_to_remove, *moves_candidate_count;
    move **moves_candidate_array;
    moves_candidate_array = color == WHITE ? moves_candidate_array_white : moves_candidate_array_black;
    moves_candidate_count = color == WHITE ? &moves_candidate_count_white : &moves_candidate_count_black;

    idx_to_remove = get_move_candidate_idx(new_move);

    if (idx_to_remove < 0 || idx_to_remove >= *moves_candidate_count)
    {
        printf("Invalid idx to remove in moves_candidate_remove(): %d\n", idx_to_remove);
        return;
    }

    if (moves_candidate_array[idx_to_remove] != NULL)
    {
        free(moves_candidate_array[idx_to_remove]);
        moves_candidate_array[idx_to_remove] = NULL;
    }

    for (i = idx_to_remove; i < *moves_candidate_count; i++)
    {
        moves_candidate_array[i] = moves_candidate_array[i + 1];
    }

    moves_candidate_array[*moves_candidate_count - 1] = NULL;
    *moves_candidate_count = *moves_candidate_count - 1;
}

void moves_candidate_remove_all(piece_color color)
{
    int i, *moves_candidate_count;
    move **moves_candidate_array;
    moves_candidate_array = color == WHITE ? moves_candidate_array_white : moves_candidate_array_black;
    moves_candidate_count = color == WHITE ? &moves_candidate_count_white : &moves_candidate_count_black;

    for (i = 0; i < *moves_candidate_count; i++)
    {
        if (moves_candidate_array[i] != NULL)
        {
            free(moves_candidate_array[i]);
            moves_candidate_array[i] = NULL;
        }
    }

    *moves_candidate_count = 0;
}

void moves_candidate_print_all(piece_color color)
{
    int i, *moves_candidate_count;
    move **moves_candidate_array;
    moves_candidate_array = color == WHITE ? moves_candidate_array_white : moves_candidate_array_black;
    moves_candidate_count = color == WHITE ? &moves_candidate_count_white : &moves_candidate_count_black;
    for (i = 0; i < *moves_candidate_count; i++)
    {
        move *new_move = moves_candidate_array[i];
        printf("---[MOVE CANDIDATE INFO]: start: %d, "
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

void moves_candidate_generate(piece_color color)
{
    moves_candidate_remove_all(color);

    int i, score, *moves_candidate_count, *moves_possible_count;
    move **moves_candidate_array, **moves_possible_array;
    moves_candidate_array = color == WHITE ? moves_candidate_array_white : moves_candidate_array_black;
    moves_candidate_count = color == WHITE ? &moves_candidate_count_white : &moves_candidate_count_black;
    moves_possible_array = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    moves_possible_count = color == WHITE ? &moves_possible_count_white : &moves_possible_count_black;

    for (i = 0; i < *moves_possible_count; i++)
    {
        score = 0;
        score = evaluate_move(moves_possible_array[i]);
        moves_possible_array[i]->score = score;
        if (((color == WHITE && score >= 0) || (color == BLACK && score <= 0)) && is_move_safe(moves_possible_array[i]))
        {
            moves_candidate_add(moves_possible_array[i]);
        }
    }
}

move *moves_candidate_get_best(piece_color color)
{
    int i, best_idx = 0, *moves_candidate_count;
    move **moves_candidate_array;
    moves_candidate_count = color == WHITE ? &moves_candidate_count_white : &moves_candidate_count_black;
    moves_candidate_array = color == WHITE ? moves_candidate_array_white : moves_candidate_array_black;
    for (i = 0; i < *moves_candidate_count; i++)
    {
        if ((color == WHITE && moves_candidate_array[best_idx]->score < moves_candidate_array[i]->score) ||
            (color == BLACK && moves_candidate_array[best_idx]->score > moves_candidate_array[i]->score))
        {
            best_idx = i;
        }
    }
    return moves_candidate_array[best_idx];
}

int get_move_candidate_idx(move *new_move)
{
    int i, *moves_candidate_count;
    move **moves_candidate_array;
    moves_candidate_array = new_move->color == WHITE ? moves_candidate_array_white : moves_candidate_array_black;
    moves_candidate_count = new_move->color == WHITE ? &moves_candidate_count_white : &moves_candidate_count_black;

    for (i = 0; i < *moves_candidate_count; i++)
    {
        if (moves_candidate_array[i]->square_start == new_move->square_start &&
        moves_candidate_array[i]->square_end == new_move->square_end &&
        moves_candidate_array[i]->color == new_move->color &&
        moves_candidate_array[i]->type == new_move->type &&
        moves_candidate_array[i]->castle == new_move->castle &&
        moves_candidate_array[i]->capture == new_move->capture &&
        moves_candidate_array[i]->pawn_promotion == new_move->pawn_promotion)
        {
            return i;
        }
    }
    return -1;
}

int evaluate_board()
{
    int score = 0;
    score += evaluate_board_material();

    return score;
}

int evaluate_board_material()
{
    int score = 0;

    score = count_set_bits(WHITE, PAWN) +
            count_set_bits(WHITE, KNIGHT) * 3 +
            count_set_bits(WHITE, BISHOP) * 3 +
            count_set_bits(WHITE, ROOK) * 5 +
            count_set_bits(WHITE, QUEEN) * 9
            -
            count_set_bits(BLACK, PAWN) -
            count_set_bits(BLACK, KNIGHT) * 3 -
            count_set_bits(BLACK, BISHOP) * 3 -
            count_set_bits(BLACK, ROOK) * 5 -
            count_set_bits(BLACK, QUEEN) * 9;
    return score;
}

int evaluate_move(move *new_move)
{
    int score = 0;
    score += evaluate_move_material(new_move);

    return score;
}

int evaluate_move_material(move *new_move)
{
    int score_before = evaluate_board_material();
    update_bitboard(new_move, true);
    int score_after = evaluate_board_material();
    update_bitboard(new_move, false);

    return score_after - score_before;
}




