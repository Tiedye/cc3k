#pragma once


#include "Stage.h"

typedef struct _win_st WINDOW;

class Menu : public Stage {
public:
    Menu(const std::shared_ptr<State> &state);

private:
    void show_info(WINDOW*win, Race*race);

    int run(Game &game) override;

    int last_race {-1};

    const int next;
};



