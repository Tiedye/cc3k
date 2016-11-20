#pragma once


#include "State.h"

class Stage {
public:
    Stage(State &state);

    virtual void start() = 0;

protected:
    State &state;
};


