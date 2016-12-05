#include "LocalAttackPlayer.h"

#include <cstdlib>
#include <list>
#include <algorithm>

#include "../action/Action.h"
#include "../State.h"
#include "../stage/Dungeon.h"
#include "../loader/Loader.h"

using namespace std;

const LocalAttackPlayer::ActionAndTarget LocalAttackPlayer::getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions,
                                                                    const std::shared_ptr<State> &state) {
    for (auto &actionAndRange:actions) {
        if (actionAndRange.action->actionType == Action::ATTACK) {
            for (auto p:actionAndRange.range) {
                for (auto &entity:state->getCurrentDungeon()->getEntitiesAt(p)) {
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
    return decorated->getAction(character, actions, state);
}