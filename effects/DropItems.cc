#include "DropItems.h"

#include "../State.h"
#include "../stage/Dungeon.h"

using namespace std;

void DropItems::notify(EventInfo &info) {
    int choice {rand() % totalWeight};
    for (auto wi:weightsAndItems){
        if (choice < wi.first) {
            Position position {info.primary->asEntity()->getPosition()};
            auto stateLocked = state.lock();
            for (auto item:wi.second) {
                shared_ptr<Item> newItem {stateLocked->library.getAnItem(item)};
                newItem->move(position);
                stateLocked->currentDungeon->addEntity(newItem);
            }
        } else {
            choice -= wi.first;
        }
    }
}

const vector<EventType> DropItems::listeningFor() const {
    return eventTypes;
}
const vector<EventType> DropItems::eventTypes {DESTROYED_DONE};

void DropItems::addItem(int weight, std::vector<int> &item) {
    weightsAndItems.emplace(weight, item);
    totalWeight += 0;
}

DropItems::DropItems(const weak_ptr<State> &state) : state(state) {}

