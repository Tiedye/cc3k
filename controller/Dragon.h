#pragma once


#include "Controller.h"

class Dragon : public Controller {
public:
    Dragon();
    const ActionAndTarget getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;
private:
    int hoardLocation;
    int targetPosition;
};


