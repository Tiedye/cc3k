#include <algorithm>
#include "Guard.h"

#include "../entity/Character.h"
#include "../action/Action.h"
#include "../stage/Dungeon.h"

using namespace std;

const Controller::ActionAndTarget Guard::get_action(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    Position current_position {character->get_position()};
    auto dungeon_locked = state->current_dungeon.lock();
    Position positions[]{
            {0, 0},
            {1, 1},
            {1, 0},
            {1, -1},
            {0, 1},
            {0, -1},
            {-1, 1},
            {-1, 0},
            {-1, -1}
    };
    Position &guarded_location_ref = character->ai_get_position(guarded_location);
    if(guarded_location_ref == Position{-1, -1}) {
        for(auto p:positions) {
            auto entities = dungeon_locked->get_entities_at(p+current_position, character);
            if (entities.size() && entities.front()->is_a(guarded_type)) {
                guarded_location_ref = p+current_position;
                break;
            }
        }
    }
    auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->action_type == Action::ATTACK; });
    vector<Position> options;
    if(guarded_location_ref != Position{-1, -1}) {
        do {
            if (selection != actions.end()) {
                for (auto p:positions) {
                    auto entity = dungeon_locked->get_entity_at(p + guarded_location_ref);
                    if (entity && any_of(from.begin(), from.end(), [&entity](int a){ return entity->is_a(a);}) && find(selection->range.begin(), selection->range.end(), p + guarded_location_ref) != selection->range.end()) {
                        options.push_back(p + guarded_location_ref);
                    }
                }
                if (!options.empty()) break;
                for (auto p:positions) {
                    auto entity = dungeon_locked->get_entity_at(p + current_position);
                    if (entity && any_of(from.begin(), from.end(), [&entity](int a){ return entity->is_a(a);}) && find(selection->range.begin(), selection->range.end(), p + current_position) != selection->range.end()) {
                        options.push_back(p + current_position);
                    }
                }
                if (!options.empty()) break;
            }
            selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->action_type == Action::MOVE; });
            if (selection != actions.end()) {
                for (auto p:positions) {
                    if (find(selection->range.begin(), selection->range.end(), p + guarded_location_ref) != selection->range.end()) {
                        options.push_back(p + guarded_location_ref);
                    }
                }
            }
        } while(false);
        if (!options.empty()) {
            ActionAndTarget action_and_target;
            action_and_target.action = selection->action;
            action_and_target.target = options[uniform_int_distribution<>(0,options.size()-1)(state->gen)];
            return action_and_target;
        }
    }
    ActionAndTarget target;
    target.action = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->action_type == Action::PASS; })->action;
    return target;
}

Guard::Guard(const int guarded_type, const vector<int> &from) : guarded_type{guarded_type}, from{from}, guarded_location{HasAIData::ai_reserve_id()}, target_position{HasAIData::ai_reserve_id()} {}
