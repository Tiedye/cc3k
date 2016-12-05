#pragma once

#include <random>
#include "loader/Library.h"
#include "display/DungeonRenderer.h"

class Character;
class Loader;
class Dungeon;

struct State: public HasAIData {
    State();

    Library library;
    std::shared_ptr<Character> player;
    std::shared_ptr<Loader> loader;
    std::weak_ptr<Dungeon> currentDungeon;
    std::shared_ptr<DungeonRenderer> dungeonRenderer;

    std::shared_ptr<Dungeon> getCurrentDungeon();

    std::mt19937 gen;
};