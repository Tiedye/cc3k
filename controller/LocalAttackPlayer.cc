#include "LocalAttackPlayer.h"

#include <cstdlib>
#include <list>
#include <algorithm>

#include "../action/Action.h"
#include "../State.h"
#include "../stage/Dungeon.h"
#include "../loader/Loader.h"

using namespace std;

const LocalAttackPlayer::ActionAndTarget LocalAttackPlayer::get_action(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions,
                                                                    const std::shared_ptr<State> &state) {
    for (auto &action_and_range:actions) {
        if (action_and_range.action->action_type == Action::ATTACK) {
            for (auto p:action_and_range.range) {
                for (auto &entity:state->get_current_dungeon()->get_entities_at(p)) {
                    if (entity->is_a(state->loader->get_id("player"))) {
                        ActionAndTarget at;
                        at.action = action_and_range.action;
                        at.target_entity = entity;
                        return at;
                    }
                }
            }
        }
    }
    return decorated->get_action(character, actions, state);
}