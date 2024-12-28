#include "chess_engine.h"

void set_up()
{
    init_moves_possible();
    init_moves_made();
    init_moves_candidate();
    init_boards();
}

void tear_down()
{
    moves_possible_remove_all(WHITE);
    moves_possible_remove_all(BLACK);
    moves_made_remove_all(WHITE);
    moves_made_remove_all(BLACK);
    moves_candidate_remove_all(WHITE);
    moves_candidate_remove_all(BLACK);
    free(bitboards);
}

void create_move(int start, int end, piece_color color, piece_type type, int castle, int capture, int pawn_promotion)
{

    move *np = malloc(sizeof(move));
    if (np == NULL) {printf("malloc move failed"); exit(1);}

    np->square_start = start;
    np->square_end = end;
    np->color = color;
    np->type = type;
    np->castle = castle;
    np->capture = capture;
    np->pawn_promotion = pawn_promotion;

    move **ma = color == WHITE ? moves_possible_array_white : moves_possible_array_black;
    int *mc = color == WHITE ? &moves_possible_count_white : &moves_possible_count_black;
    ma[*mc] = np;
    *mc = *mc + 1;
}

move *create_return_move(int start, int end, piece_color color, piece_type type, int castle, int capture, int pawn_promotion)
{
    move *np = malloc(sizeof(move));
    if (np == NULL) {printf("malloc move failed"); exit(1);}

    np->square_start = start;
    np->square_end = end;
    np->color = color;
    np->type = type;
    np->castle = castle;
    np->capture = capture;
    np->pawn_promotion = pawn_promotion;

    return np;
}

void print_move(move* new_move)
{
    printf("---[MOVE INFO]: start: %d, "
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

void test_evaluation()
{
    set_up();

    game_loop();

    tear_down();
}




