#pragma once

#include "entity/Character.h"
#include "loader/Loader.h"

struct State {
    Character player {};
    Loader loader {};
};