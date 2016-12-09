#include "EndScreen.h"

#include "../State.h"

#include <curses.h>

using namespace std;

EndScreen::EndScreen(const std::shared_ptr<State> &state, const int return_id) : Stage(state), return_id{return_id} {}

void print_centered(WINDOW*win, string str, int row){
    int width {getmaxx(win)};
    mvwaddstr(win, row, (width - str.length())/2, str.c_str());
}

int EndScreen::run(Game &game) {
    initscr();
    clear();
    noecho();
    int height {getmaxy(stdscr)};
    int row {height/2};
    if (state->player->is_dead()) {
        print_centered(stdscr, "You Died.", row);
    } else {
        print_centered(stdscr, "You Complete the Game.", row);
    }
    ++row;
    print_centered(stdscr, "Your score:", row);
    ++row;
    print_centered(stdscr, to_string(state->player->current_score()), row);
    int c;
    int next_stage {-1};
    bool needs_input {true};
    while (needs_input) {
        c = getch();
        switch (c) {
            case 'q':
                needs_input = false;
                break;
            case 'r':
                needs_input = false;
                next_stage = return_id;
            default:
                break;
        }
    }
    endwin();
    return next_stage;
}
