#pragma once


#include "Controller.h"

class StateConditional : public Controller {
public:
    StateConditional(const int state_id, const std::shared_ptr<Controller> &true_controller, const std::shared_ptr<Controller> &false_controller);
    const ActionAndTarget get_action(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;
private:
    const int state_id;
    const std::shared_ptr<Controller> true_controller;
    const std::shared_ptr<Controller> false_controller;
};


