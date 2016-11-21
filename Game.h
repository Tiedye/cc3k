#pragma once

#include <map>
#include <stack>
#include "stage/Stage.h"
#include "entity/Entity.h"
#include "State.h"

class Game {
public:
    void setInitialStage(int stage);

    void addStage(int group, Stage *stage);
    void removeState(int id);
    void removeStates(int group);

    void addEntityClass(int id, Entity *entity);
    State & getState();
    void start();
private:
    std::map<int, Stage*> stages;
    Stage *currentStage;
    State state;
};


