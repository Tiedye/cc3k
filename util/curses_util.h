#pragma once

#include <string>

typedef struct _win_st WINDOW;
void print_centered(WINDOW*win, std::string str, int row);