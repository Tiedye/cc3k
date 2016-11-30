#pragma once

#include "Controller.h"

class PlayerController : public Controller {
public:
    const ActionAndTarget getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions,
                                    const std::shared_ptr<State> &state) override;
};



