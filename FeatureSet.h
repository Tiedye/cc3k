#pragma once

#include <vector>
#include "Listener.h"
#include "StatModifier.h"
#include "Action.h"

struct FeatureSet {
    std::vector<Listener*> listeners;
    std::vector<StatModifier> statModifiers;
    std::vector<Action*> actions;
};



