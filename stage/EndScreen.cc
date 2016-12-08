#include "EndScreen.h"

#include "../State.h"

#include <curses.h>

using namespace std;

EndScreen::EndScreen(const std::shared_ptr<State> &state, const int returnId) : Stage(state), returnId{returnId} {}

void printCentered(WINDOW*win, string str, int row){
    int width {getmaxx(win)};
    mvwaddstr(win, row, (width - str.length())/2, str.c_str());
}

int EndScreen::run(Game &game) {
    initscr();
    clear();
    noecho();
    int height {getmaxy(stdscr)};
    int row {height/2};
    if (state->player->isDead()) {
        printCentered(stdscr, "You Died.", row);
    } else {
        printCentered(stdscr, "You Complete the Game.", row);
    }
    ++row;
    printCentered(stdscr, "Your score:", row);
    ++row;
    printCentered(stdscr, to_string(state->player->currentScore()), row);
    int c;
    int nextStage {-1};
    bool needsInput {true};
    while (needsInput) {
        c = getch();
        switch (c) {
            case 'q':
                needsInput = false;
                break;
            case 'r':
                needsInput = false;
                nextStage = returnId;
            default:
                break;
        }
    }
    endwin();
    return nextStage;
}
