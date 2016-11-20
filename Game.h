#pragma once

#include <unordered_map>
#include <stack>
#include "Stage.h"
#include "entity/Entity.h"
#include "State.h"

class Game {
public:
    void setInitialStage(int stage);
    void addStage(Stage *stage);
    void addEntityClass(int id, Entity *entity);
    State & getState();
    void start();
private:
    std::unordered_map<int, Stage*> stages;
    std::stack<Stage*> stageStack;
};


