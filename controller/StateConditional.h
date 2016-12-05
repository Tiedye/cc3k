#pragma once


#include "Controller.h"

class StateConditional : public Controller {
public:
    StateConditional(const int stateId, const std::shared_ptr<Controller> &trueController, const std::shared_ptr<Controller> &falseController);
    const ActionAndTarget getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;
private:
    const int stateId;
    const std::shared_ptr<Controller> trueController;
    const std::shared_ptr<Controller> falseController;
};


