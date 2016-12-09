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
    std::weak_ptr<Dungeon> current_dungeon;
    std::shared_ptr<DungeonRenderer> dungeon_renderer;

    std::shared_ptr<Dungeon> get_current_dungeon();

    std::mt19937 gen;
};