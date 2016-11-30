#pragma once


#include "../State.h"
class Game;

class Stage {
public:
    Stage(const std::shared_ptr<State> &state, int id = -1);

    virtual int run(Game &game) = 0;

    virtual ~Stage() = default;

    const int id;

protected:
    const std::shared_ptr<State> state;
};


