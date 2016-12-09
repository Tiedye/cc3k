#pragma once


#include "Controller.h"

class Guard : public Controller {
public:
    Guard(const int guarded_type, const std::vector<int> &from);
    const ActionAndTarget get_action(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;
private:
    const int guarded_type;
    const std::vector<int> from;

    const int guarded_location;
    const int target_position;
};


