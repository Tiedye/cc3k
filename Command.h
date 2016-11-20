#pragma once

#include "util/Position.h"

struct Command {
    enum Type {PASS, MOVE, ATTACK, INTERACT};
    Type type;
    int selection;
    Position position;
};



