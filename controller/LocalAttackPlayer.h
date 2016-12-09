#pragma once

#include "ControllerDecorator.h"

struct LocalAttackPlayer : public ControllerDecorator {
    using ControllerDecorator::ControllerDecorator;
    const ActionAndTarget get_action(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions,
                                    const std::shared_ptr<State> &state);
};