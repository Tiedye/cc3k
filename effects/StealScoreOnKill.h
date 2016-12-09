#pragma once


#include "../event/Listener.h"

class StealScoreOnKill : public Listener {
public:
    StealScoreOnKill();
    void notify(EventInfo &info) override;
    const std::vector<EventType> listening_for() const override;
};


