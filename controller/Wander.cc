#include <random>
#include <algorithm>
#include "Wander.h"

#include "../action/Action.h"
#include "../State.h"
#include "../display/ConsoleDungeonIO.h"

using namespace std;

const Controller::ActionAndTarget
Wander::get_action(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    for (auto& action_and_range:actions) {
        std::shared_ptr<Action> action = action_and_range.action;
        if (action->action_type == Action::MOVE && !action_and_range.range.empty()) {
            ActionAndTarget action_and_target;
            action_and_target.action = action;
            uniform_int_distribution<> opts(0, action_and_range.range.size()-1);
            action_and_target.target = action_and_range.range[opts(state->gen)];
            return action_and_target;
        }
    }
    ActionAndTarget pass_action;
    auto pass = find_if(actions.begin(), actions.end(), [](ActionAndRange ar){ return ar.action->action_type == Action::PASS;});
    pass_action.action = pass->action;
    return pass_action;
}
