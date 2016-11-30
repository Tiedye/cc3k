#pragma once


#include "Stage.h"

class Menu : Stage {
public:
    Menu(const std::shared_ptr<State> &state, int id);

private:
    int run(Game &game) override;
};



