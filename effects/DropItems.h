#pragma once

#include "../event/Listener.h"
#include "../entity/Item.h"
class State;
#include <map>

class DropItems : public Listener {
public:
    DropItems(const std::weak_ptr<State> &state);

    void addItem(int weight, std::vector<int> &item);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listeningFor() const override;

private:
    std::weak_ptr<State> state;
    static const std::vector<EventType> eventTypes;
    int totalWeight {0};
    std::multimap<int, std::vector<int>> weightsAndItems;
};



