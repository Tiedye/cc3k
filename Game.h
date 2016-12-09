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

    void set_initial_stage(int stage);

    void add_stage(int group, const std::shared_ptr<Stage> &stage);
    void remove_state(int id);
    void remove_states(int group);

    void start();

    static int get_id();
    static int next_id();
private:
    static int ids;
    std::map<int, std::shared_ptr<Stage>> stages;
    std::map<int, std::vector<std::map<int, std::shared_ptr<Stage>>::iterator>> groups;
    int initial_stage;
    std::shared_ptr<State> state;
};


