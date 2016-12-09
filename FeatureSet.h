#pragma once

#include <vector>
#include <memory>
#include "util/StatModifier.h"
class Action;
class Listener;

struct FeatureSet {
    std::vector<std::shared_ptr<Listener>> listeners;
    std::vector<StatModifier> stat_modifiers;
    std::vector<std::shared_ptr<Action>> actions;
};



