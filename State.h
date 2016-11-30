#pragma once

#include "loader/Library.h"
#include "display/DungeonRenderer.h"

class Character;
class Loader;
class Dungeon;

struct State {
    State() = default;

    Library library;
    std::shared_ptr<Character> player;
    std::shared_ptr<Loader> loader;
    std::shared_ptr<Dungeon> currentDungeon;
    std::shared_ptr<DungeonRenderer> dungeonRenderer;
};