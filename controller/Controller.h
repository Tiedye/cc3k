#pragma once

#include <memory>
#include <set>
#include <utility>
#include <vector>
#include "../action/Action.h"
#include "../entity/Character.h"

struct Controller {
    struct ActionAndRange {
        std::shared_ptr<Action> action;
        std::vector<Position> range;
    };
    struct ActionAndTarget {
        std::shared_ptr<Action> action;
        Position target;
        std::shared_ptr<Entity> targetEntity;
    };

    virtual const ActionAndTarget getAction(std::shared_ptr<Character> character, const std::vector<ActionAndRange> &actions) = 0;
};


