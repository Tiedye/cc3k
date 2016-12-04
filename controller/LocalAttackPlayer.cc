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
    auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar){ return ar.action->actionType == Action::ATTACK;});
    if (selection != actions.end()) {
        for (auto p:selection->range) {
            for (auto& entity:state->currentDungeon->getEntitiesAt(p)) {
                if (entity->isA(state->loader->getId("player"))) {
                    ActionAndTarget at;
                    at.action = selection->action;
                    at.targetEntity = entity;
                    return at;
                }
            }
        }
    }
    return decorated->getAction(character, actions, state);
}