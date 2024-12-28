#include "chess_engine.h"

game_state state = PLAYING;
piece_color turn = WHITE;

move **moves_made_array_white = NULL;
move **moves_made_array_black = NULL;
int moves_made_count_white = 0;
int moves_made_count_black = 0;
int safe_move_count_white = 0;
int safe_move_count_black = 0;

void init_moves_made()
{
    int i;
    moves_made_array_white = malloc(sizeof(move *) * MAX_MOVES);
    moves_made_array_black = malloc(sizeof(move *) * MAX_MOVES);

    if (moves_made_array_white == NULL || moves_made_array_black == NULL)
    {
        printf("Malloc failed for moves made array. \n");
        exit(1);
    }

    for (i = 0; i < MAX_MOVES; i++)
    {
        moves_made_array_white[i] = NULL;
        moves_made_array_black[i] = NULL;
    }

    if (DEBUG_MODE) {
        printf("---[INIT MOVES MADE]---\n"
               "moves_made_array_white: %p\n"
               "moves_made_array_black: %p\n",
               moves_made_array_white,
               moves_made_array_black);
    }
}

void moves_made_add(move* old_move)
{
    if (!is_move_safe(old_move)) { return; }
    update_bitboard(old_move, true);

    int *moves_made_count;
    move **moves_made_array;
    moves_made_array = old_move->color == WHITE ? moves_made_array_white : moves_made_array_black;
    moves_made_count = old_move->color == WHITE ? &moves_made_count_white : &moves_made_count_black;

    move *new_move = malloc(sizeof(move));
    if (new_move == NULL) {
        printf("Memory allocation failed for moves made add.\n");
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



    moves_made_array[*moves_made_count] = new_move;
    *moves_made_count = *moves_made_count + 1;
}

void moves_made_remove(move *new_move)
{
    int square_start = new_move->square_start;
    int square_end = new_move->square_end;
    piece_color color = new_move->color;
    piece_type type = new_move->type;
    int castle = new_move->castle;
    int capture = new_move->capture;
    int pawn_promotion = new_move->pawn_promotion;

    int i, idx_to_remove, *moves_made_count;
    move **moves_made_array;
    moves_made_array = color == WHITE ? moves_made_array_white : moves_made_array_black;
    moves_made_count = color == WHITE ? &moves_made_count_white : &moves_made_count_black;

    idx_to_remove = get_move_made_idx(square_start, square_end, color, pawn_promotion);

    if (idx_to_remove < 0 || idx_to_remove >= *moves_made_count)
    {
        printf("Invalid idx to remove in moves_made_remove(): %d\n", idx_to_remove);
        return;
    }

    update_bitboard(new_move, false);

    if (DEBUG_MODE)
    {
        printf("---[MOVE MADE REMOVE]---\n"
               "start: %d,\n"
               "end: %d,\n"
               "color: %d, \n"
               "type: %d, \n"
               "castle: %d, \n"
               "capture: %d, \n"
               "idx to remove: %d,\n"
               "move count: %d \n",
               square_start,
               square_end,
               color,
               type,
               castle,
               capture,
               idx_to_remove,
               *moves_made_count);
    }

    if (moves_made_array[idx_to_remove] != NULL)
    {
        free(moves_made_array[idx_to_remove]);
        moves_made_array[idx_to_remove] = NULL;
    }

    for (i = idx_to_remove; i < *moves_made_count; i++)
    {
        moves_made_array[i] = moves_made_array[i + 1];
    }

    moves_made_array[*moves_made_count - 1] = NULL;
    *moves_made_count = *moves_made_count - 1;
}

void moves_made_remove_all(piece_color color)
{
    int i, *moves_made_count;
    move **moves_made_array;
    moves_made_array = color == WHITE ? moves_made_array_white : moves_made_array_black;
    moves_made_count = color == WHITE ? &moves_made_count_white : &moves_made_count_black;

    if (DEBUG_MODE) {
        printf("---[MOVES MADE REMOVE ALL]---\n"
               "color: %d\n"
               "moves_made_count_white: %d\n"
               "moves_made_count_black: %d\n"
               "moves_made_array address: %p\n",
               color,
               moves_made_count_white,
               moves_made_count_black,
               moves_made_array);
    }

    for (i = 0; i < *moves_made_count; i++)
    {
        if (moves_made_array[i] != NULL)
        {
            free(moves_made_array[i]);
            moves_made_array[i] = NULL;
        }

    }


    *moves_made_count = 0;
}

void moves_made_print_all(piece_color color)
{
    int i, moves_made_count;
    move **moves_made_array;
    moves_made_array = color == WHITE ? moves_made_array_white : moves_made_array_black;
    moves_made_count = color == WHITE ? moves_made_count_white : moves_made_count_black;
    for (i = 0; i < moves_made_count; i++)
    {
        move *new_move = moves_made_array[i];
        printf("[MOVES MADE ARRAY] %s's move #%d: start idx: %d, end idx: %d, color: %d, "
               "type: %d, castle: %d, capture: %d \n",
            color == 0 ? "white" : "black", i+1, new_move->square_start, new_move->square_end,
            new_move->color, new_move->type, new_move->castle, new_move->capture);
    }
}

void moves_made_print_last(piece_color color)
{
    int moves_made_count;
    move **moves_made_array;
    moves_made_array = color == WHITE ? moves_made_array_white : moves_made_array_black;
    moves_made_count = color == WHITE ? moves_made_count_white : moves_made_count_black;

    move *new_move = moves_made_array[moves_made_count - 1];

    int capture = new_move->capture;
    int pawn_promotion = new_move->pawn_promotion;
    char piece = " NBRQK"[new_move->type];
    char pp_piece = " NBRQK"[new_move->pawn_promotion];

    int start_rank = get_rank_of_bit(new_move->square_start) + 1;
    char start_file = get_file_of_bit(new_move->square_start) + 'a';
    int end_rank = get_rank_of_bit(new_move->square_end) + 1;
    char end_file = get_file_of_bit(new_move->square_end) + 'a';

    printf("----------[MOVE # %d MADE BY %s]----------: %c%c%d%c%c%d%c%c\n",
           moves_made_count,
           color == WHITE ? "WHITE" : "BLACK",
           piece,
           start_file, start_rank,
           capture != -1 ? 'x' : ' ',
           end_file, end_rank,
           pawn_promotion != -1 ? '=' : ' ',
           pp_piece);
}

bool is_move_safe(move *new_move)
{
    bool check = false;
    int square_start = new_move->square_start;
    int square_end = new_move->square_end;
    piece_color color = new_move->color;
    piece_type type = new_move->type;
    int castle = new_move->castle;
    int capture = new_move->capture;
    int pawn_promotion = new_move->pawn_promotion;

    Bitboard local_bitboards = *bitboards;

    update_bitboard(new_move, true);
    moves_possible_generate(!color);
    check = is_in_check(color);
    update_bitboard(new_move, false);

    *bitboards = local_bitboards;

    return !check;
}

bool has_piece_moved(int square_start, piece_color color, piece_type type) {
    int i, moves_made_count;
    move **moves_made_array;
    moves_made_array = color == WHITE ? moves_made_array_white : moves_made_array_black;
    moves_made_count = color == WHITE ? moves_made_count_white : moves_made_count_black;
    for (i = 0; i < moves_made_count; i++)
    {
        if (moves_made_array[i]->square_start == square_start && moves_made_array[i]->type == type)
        {
            return true;
        }
    }
    return false;
}

int get_move_made_idx(int square_start, int square_end, piece_color color, int pawn_promotion)
{
    int i, moves_made_count;
    move **moves_made_array;
    moves_made_array = color == WHITE ? moves_made_array_white : moves_made_array_black;
    moves_made_count = color == WHITE ? moves_made_count_white : moves_made_count_black;
    for (i = 0; i < moves_made_count; i++)
        if (moves_made_array[i]->square_start == square_start
            && moves_made_array[i]->square_end == square_end
            && moves_made_array[i]->pawn_promotion == pawn_promotion)
        {
            return i;
        }
    return -1;
}

void check_game_state()
{
    int i, color, type;
    int num_pieces_white = 0, num_pieces_black = 0;
    int num_non_minor_pieces_white = 0, num_non_minor_pieces_black = 0;
    int fifty_move_rule = 0, threefold_repetition_rule = 0;

    moves_possible_generate(WHITE);
    moves_possible_generate(BLACK);

    bool white_in_check = is_in_check(WHITE);
    bool black_in_check = is_in_check(BLACK);

    move **mpaw = moves_possible_array_white;
    int *mpcw = &moves_possible_count_white;
    move **mpab = moves_possible_array_black;
    int *mpcb = &moves_possible_count_black;

    move **mmaw = moves_made_array_white;
    int *mmcw = &moves_made_count_white;
    move **mmab = moves_made_array_black;
    int *mmcb = &moves_made_count_black;

    for (type = 0; type < 6; type++)
    {
        num_pieces_white = num_pieces_white + count_set_bits(WHITE, type);
        num_pieces_black = num_pieces_black + count_set_bits(BLACK, type);
    }

    num_non_minor_pieces_white = count_set_bits(WHITE, PAWN) + count_set_bits(WHITE, ROOK) + count_set_bits(WHITE, QUEEN);
    num_non_minor_pieces_black = count_set_bits(BLACK, PAWN) + count_set_bits(BLACK, ROOK) + count_set_bits(BLACK, QUEEN);

    if (*mmcw >= 50)
    {
        for (i = *mmcw - 1; i >= *mmcw - 50; i--)
        {
            if (mmaw[i]->type == PAWN || mmaw[i]->capture != -1) { break; }
        }
        fifty_move_rule = 1;
    }

    if (*mmcb >= 50)
    {
        for (i = *mmcb - 1; i >= *mmcb - 50; i--)
        {
            if (mmab[i]->type == PAWN || mmab[i]->capture != -1) { break; }
        }
        fifty_move_rule = 1;
    }

    game_state new_state = PLAYING;

    if (white_in_check && moves_candidate_count_white == 0) { new_state = WHITE_CHECKMATED; }
    else if (black_in_check && moves_candidate_count_black == 0) { new_state = BLACK_CHECKMATED; }
    else if (moves_candidate_count_white == 0 || moves_candidate_count_black == 0) { new_state = STALEMATE; }
    else if ((num_pieces_white == 1 && num_pieces_black == 1) ||
             (num_pieces_white == 2 && num_pieces_black == 2 &&
              num_non_minor_pieces_white == 0 && num_non_minor_pieces_black == 0)) {
        new_state = INSUFFICIENT_MATERIAL;
    }
    else if (threefold_repetition_rule) { new_state = THREEFOLD_REPETITION; }
    else if (fifty_move_rule == 1) { new_state = FIFTY_MOVE_RULE; }

    if (new_state != PLAYING)
    {
        state = new_state;

        switch (state)
        {
            case WHITE_CHECKMATED:
                printf("Checkmate. Black wins.\n");
                break;
            case BLACK_CHECKMATED:
                printf("Checkmate. White wins.\n");
                break;
            case STALEMATE:
                printf("Stalemate. It's a draw.\n");
                break;
            case INSUFFICIENT_MATERIAL:
                printf("Insufficient material. It's a draw.\n");
                break;
            case THREEFOLD_REPETITION:
                printf("Threefold repetition. It's a draw.\n");
                break;
            case FIFTY_MOVE_RULE:
                printf("Fifty-move rule. It's a draw.\n");
                break;
            default:
                break;
        }
    }
}

move *get_input() {
    int i, s, e, pp;
    int mpc = turn == WHITE ? moves_possible_count_white : moves_possible_count_black;
    move **mpa = turn == WHITE ? moves_possible_array_white : moves_possible_array_black;
    printf("Enter start idx, end idx, pawn promotion:\n");
    scanf("%d%d%d", &s, &e, &pp);

    for (i = 0; i < mpc; i++)
    {
        move *m = mpa[i];
        if (m->square_start == s && m->square_end == e && m->pawn_promotion == pp) { return m; }
    }
    return NULL;

}

void game_loop()
{
    init_moves_possible();
    init_moves_made();
    init_moves_candidate();
    init_boards();

    while (state == PLAYING)
    {
        clear_board();
        update_board();
        print_board();

        int s, e, t, cas, cap, pp, midx, flag, *mmc;
        move **mpa, *nm;
        mpa = turn == WHITE ? moves_possible_array_white : moves_possible_array_black;
        mmc = turn == WHITE ? &moves_made_count_white : &moves_made_count_black;
        flag = *mmc;
        while (1)
        {
            moves_possible_generate(turn);
            moves_candidate_generate(turn);

            move *best_move = moves_candidate_get_best(turn);
            printf("Computer suggestion: start idx: %d end idx: %d pp: %d\n", best_move->square_start, best_move->square_end, best_move->pawn_promotion);

            nm = get_input();
            s = nm->square_start, e = nm->square_end, t = nm->type, cas = nm->castle, cap = nm->capture, pp = nm->pawn_promotion;
            moves_made_add(nm);
            if (flag == *mmc) { printf("invalid move\n"); continue; }
            else { break; }
        }

        check_game_state();
        turn = turn == WHITE ? BLACK : WHITE;
    }
}



