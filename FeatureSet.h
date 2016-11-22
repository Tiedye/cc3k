#pragma once

#include <vector>
#include <memory>
#include "StatModifier.h"
class Action;
class Listener;

struct FeatureSet {
    std::vector<std::shared_ptr<Listener>> listeners;
    std::vector<StatModifier> statModifiers;
    std::vector<std::shared_ptr<Action>> actions;
};



