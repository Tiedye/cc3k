#pragma once

#include <string>

class FeatureSet;

struct Race {
    std::string name;
    std::string description;
    std::string shortCut;
    FeatureSet *featureSet;
    int id;
};



