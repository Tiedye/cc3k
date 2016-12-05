#include <random>
#include <algorithm>
#include "Wander.h"

#include "../action/Action.h"
#include "../State.h"
#include "../display/ConsoleDungeonIO.h"

using namespace std;

const Controller::ActionAndTarget
Wander::getAction(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    for (auto& actionAndRange:actions) {
        std::shared_ptr<Action> action = actionAndRange.action;
        if (action->actionType == Action::MOVE && !actionAndRange.range.empty()) {
            ActionAndTarget actionAndTarget;
            actionAndTarget.action = action;
            uniform_int_distribution<> opts(0, actionAndRange.range.size()-1);
            actionAndTarget.target = actionAndRange.range[opts(state->gen)];
            return actionAndTarget;
        }
    }
    ActionAndTarget passAction;
    auto pass = find_if(actions.begin(), actions.end(), [](ActionAndRange ar){ return ar.action->actionType == Action::PASS;});
    passAction.action = pass->action;
    return passAction;
}
