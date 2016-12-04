#pragma once

#include <map>
#include <memory>
#include <vector>
#include "../util/Position.h"

class Entity;

class HasAIData {
public:
private:
    struct Data {
        union {
            std::vector<std::shared_ptr<Entity>> entities;
            std::vector<int> integers;
            std::vector<float > floats;
            std::vector<double > doubles;
            std::vector<Position> positions;
            std::shared_ptr<Entity> entity;
            int integerValue;
            float floatValue;
            double doubleValue;
            Position position;
        };
    };

    std::map<int, Data> data;
};


