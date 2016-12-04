#pragma once

#include "ControllerDecorator.h"

class Merchant : public ControllerDecorator {
public:
    Merchant(const std::shared_ptr<Controller> &decorated, const std::shared_ptr<State> &state);
    const ActionAndTarget getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;

private:
    int merchantsHostileDataId;
};