#pragma once


#include "Controller.h"

struct Wander : public Controller {
    const ActionAndTarget get_action(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;
};


