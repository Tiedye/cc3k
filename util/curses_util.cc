#include "curses_util.h"

#include <curses.h>

void print_centered(WINDOW*win, std::string str, int row){
    int width {getmaxx(win)};
    mvwaddstr(win, row, (width - str.length())/2, str.c_str());
}