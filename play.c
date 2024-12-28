#include "chess_engine.h"

void cpu_play_cpu()
{
    init_moves_possible();
    init_moves_made();
    init_moves_candidate();
    init_boards();

    while (state == PLAYING)
    {
        move *best_move;

        moves_possible_generate(turn);
        moves_candidate_generate(turn);
        moves_possible_generate(!turn);
        moves_candidate_generate(!turn);

        best_move = moves_candidate_get_best(turn);
        moves_made_add(best_move);

        print_move(best_move);
        clear_board();
        update_board();
        print_board();

        check_game_state();
        turn = turn == WHITE ? BLACK : WHITE;
    }
}
