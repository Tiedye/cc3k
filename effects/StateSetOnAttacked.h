#pragma once

class State;
#include "../event/Listener.h"

class StateSetOnAttacked : public Listener {
public:
    StateSetOnAttacked(const int id_to_set, const int when_attacked_by, const std::weak_ptr<State> &state);
    void notify(EventInfo &info) override;
    const std::vector<EventType> listening_for() const override;
private:
    const int id_to_set;
    const int when_attacked_by;
    std::weak_ptr<State> state;
};


