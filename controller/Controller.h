#pragma once

#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "../util/Position.h"

class Action;
class State;
class Character;
class Entity;

struct Controller {
    struct ActionAndRange {
        std::shared_ptr<Action> action;
        std::vector<Position> range;
    };
    struct ActionAndTarget {
        ActionAndTarget() = default;
        std::shared_ptr<Action> action;
        Position target;
        std::shared_ptr<Entity> target_entity;
    };

    virtual ~Controller() = default;

    virtual const ActionAndTarget get_action(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions,
                                            const std::shared_ptr<State> &state) = 0;
};


