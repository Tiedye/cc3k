#pragma once


#include "../State.h"
class Game;

class Stage {
public:
    Stage(State &state, int id);

    virtual void start(Game &game) = 0;

    virtual ~Stage() = default;

protected:
    State &state;
    const int id;
};


