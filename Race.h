#pragma once

#include <string>
#include <memory>
#include <set>

class FeatureSet;

struct Race {
    std::string name;
    std::string description;
    std::string shortcut;
    std::shared_ptr<FeatureSet> featureSet;
    std::set<int> slots;
    int id;
};



