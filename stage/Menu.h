#pragma once


#include "Stage.h"

class Menu : public Stage {
public:
    Menu(const std::shared_ptr<State> &state);

private:
    int run(Game &game) override;

    int lastRace {-1};

    const int next;
};



