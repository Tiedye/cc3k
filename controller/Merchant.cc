#include "Merchant.h"

#include <list>
#include <algorithm>

#include "../action/Action.h"
#include "../stage/Dungeon.h"
#include "../loader/Loader.h"
#include "../State.h"

using namespace std;

const Controller::ActionAndTarget
Merchant::getAction(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    if (state->aiGetInteger(merchantsHostileDataId)) {
        for (auto &actionAndRange:actions) {
            if (actionAndRange.action->actionType == Action::ATTACK) {
                for (auto p:actionAndRange.range) {
                    for (auto &entity:state->currentDungeon->getEntitiesAt(p)) {
                        if (entity->isA(state->loader->getId("player"))) {
                            ActionAndTarget at;
                            at.action = actionAndRange.action;
                            at.targetEntity = entity;
                            return at;
                        }
                    }
                }
            }
        }
    }
    return decorated->getAction(character, actions, state);
}

Merchant::Merchant(const shared_ptr<Controller> &decorated, const shared_ptr<State> &state) : ControllerDecorator(decorated) {
    if (HasAIData::aiReservedId("merchants_hostile")) {
        merchantsHostileDataId = HasAIData::aiGetId("merchants_hostile");
    } else {
        merchantsHostileDataId = HasAIData::aiReserveId("merchants_hostile");
        state->aiGetInteger(merchantsHostileDataId) = false;
    }
}
