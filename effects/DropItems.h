#pragma once

#include "../event/Listener.h"
#include "../entity/Item.h"
class State;
#include <map>

class DropItems : public Listener {
public:
    DropItems(const std::weak_ptr<State> &state);

    void add_item(int weight, const std::vector<int> &item);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listening_for() const override;

private:
    std::weak_ptr<State> state;
    static const std::vector<EventType> event_types;
    int total_weight {0};
    std::multimap<int, std::vector<int>> weights_and_items;
};



