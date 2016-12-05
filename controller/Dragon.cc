#include <algorithm>
#include "Dragon.h"

#include "../entity/Character.h"
#include "../action/Action.h"

using namespace std;

const Controller::ActionAndTarget
Dragon::getAction(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    if(character->aiGetPosition(hoardLocation) == Position{-1, -1}) {
        //
    }
    ActionAndTarget target;
    target.action = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::PASS; })->action;
    return target;
}

Dragon::Dragon() {
    hoardLocation = HasAIData::aiReserveId();
    targetPosition = HasAIData::aiReserveId();
}
