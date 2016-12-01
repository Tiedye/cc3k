#pragma once

#include <map>
#include <stack>
#include <memory>
#include "stage/Stage.h"
#include "entity/Entity.h"
#include "State.h"

class Game {
public:
    Game(const std::shared_ptr<State> &state);

    void setInitialStage(int stage);

    void addStage(int group, const std::shared_ptr<Stage> &stage);
    void removeState(int id);
    void removeStates(int group);

    void start();

    static int getId();
    static int nextId();
private:
    static int ids;
    std::map<int, std::shared_ptr<Stage>> stages;
    std::map<int, std::vector<std::map<int, std::shared_ptr<Stage>>::iterator>> groups;
    int initialStage;
    std::shared_ptr<State> state;
};


