#pragma once

#include <vector>
#include "StatModifier.h"
class Action;
class Listener;

struct FeatureSet {
    std::vector<Listener*> listeners;
    std::vector<StatModifier> statModifiers;
    std::vector<Action*> actions;
};



