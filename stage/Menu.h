#pragma once


#include "Stage.h"

class Menu : public Stage {
public:
    Menu(const std::shared_ptr<State> &state, const int id, const int next);

private:
    int run(Game &game) override;

    const int next;
};



