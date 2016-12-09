#include "DropItems.h"

#include "../State.h"
#include "../stage/Dungeon.h"
#include "../display/ConsoleDungeonIO.h"

using namespace std;

void DropItems::notify(EventInfo &info) {
    int choice {uniform_int_distribution<>(0, total_weight-1)(state.lock()->gen)};
    for (auto& wi:weights_and_items){
        if (choice < wi.first) {
            Position position {info.primary->as_entity()->get_position()};
            auto state_locked = state.lock();
            for (auto& item:wi.second) {
                shared_ptr<Item> new_item {state_locked->library.get_an_item(item)};
                new_item->move(position);
                state_locked->get_current_dungeon()->add_entity(new_item);
            }
            break;
        } else {
            choice -= wi.first;
        }
    }
}

const vector<EventType> DropItems::listening_for() const {
    return event_types;
}
const vector<EventType> DropItems::event_types {DESTROYED_DONE};

void DropItems::add_item(int weight, const vector<int> &item) {
    weights_and_items.emplace(weight, item);
    total_weight += weight;
}

DropItems::DropItems(const weak_ptr<State> &state) : state(state) {}

