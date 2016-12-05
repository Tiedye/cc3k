#pragma once

class State;
#include "../event/Listener.h"

class StateSetOnAttacked : public Listener {
public:
    StateSetOnAttacked(const int idToSet, const int whenAttackedBy, const std::weak_ptr<State> &state);
    void notify(EventInfo &info) override;
    const std::vector<EventType> listeningFor() const override;
private:
    const int idToSet;
    const int whenAttackedBy;
    std::weak_ptr<State> state;
};


