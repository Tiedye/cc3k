#include <algorithm>
#include "Guard.h"

#include "../entity/Character.h"
#include "../action/Action.h"
#include "../stage/Dungeon.h"

using namespace std;

const Controller::ActionAndTarget Guard::getAction(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    Position currentPosition {character->getPosition()};
    auto dungeonLocked = state->currentDungeon.lock();
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
    Position &guardedLocationRef = character->aiGetPosition(guardedLocation);
    if(guardedLocationRef == Position{-1, -1}) {
        for(auto p:positions) {
            auto entities = dungeonLocked->getEntitiesAt(p+currentPosition, character);
            if (entities.size() && entities.front()->isA(guardedType)) {
                guardedLocationRef = p+currentPosition;
                break;
            }
        }
    }
    auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::ATTACK; });
    vector<Position> options;
    if(guardedLocationRef != Position{-1, -1}) {
        do {
            if (selection != actions.end()) {
                for (auto p:positions) {
                    auto entity = dungeonLocked->getEntityAt(p + guardedLocationRef);
                    if (entity && any_of(from.begin(), from.end(), [&entity](int a){ return entity->isA(a);}) && find(selection->range.begin(), selection->range.end(), p + guardedLocationRef) != selection->range.end()) {
                        options.push_back(p + guardedLocationRef);
                    }
                }
                if (!options.empty()) break;
                for (auto p:positions) {
                    auto entity = dungeonLocked->getEntityAt(p + currentPosition);
                    if (entity && any_of(from.begin(), from.end(), [&entity](int a){ return entity->isA(a);}) && find(selection->range.begin(), selection->range.end(), p + currentPosition) != selection->range.end()) {
                        options.push_back(p + currentPosition);
                    }
                }
                if (!options.empty()) break;
            }
            selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::MOVE; });
            if (selection != actions.end()) {
                for (auto p:positions) {
                    if (find(selection->range.begin(), selection->range.end(), p + guardedLocationRef) != selection->range.end()) {
                        options.push_back(p + guardedLocationRef);
                    }
                }
            }
        } while(false);
        if (!options.empty()) {
            ActionAndTarget actionAndTarget;
            actionAndTarget.action = selection->action;
            actionAndTarget.target = options[uniform_int_distribution<>(0,options.size()-1)(state->gen)];
            return actionAndTarget;
        }
    }
    ActionAndTarget target;
    target.action = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::PASS; })->action;
    return target;
}

Guard::Guard(const int guardedType, const vector<int> &from) : guardedType{guardedType}, from{from}, guardedLocation{HasAIData::aiReserveId()}, targetPosition{HasAIData::aiReserveId()} {}
