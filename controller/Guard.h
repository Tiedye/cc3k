#pragma once


#include "Controller.h"

class Guard : public Controller {
public:
    Guard(const int guardedType);
    const ActionAndTarget getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;
private:
    const int guardedType;
    const int guardedLocation;
    const int targetPosition;
};


