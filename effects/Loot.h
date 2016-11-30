#pragma once

#include "../event/Listener.h"

class Loot : public Listener {
public:
    Loot(int amount);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listeningFor() const override;

private:
    static const std::vector<EventType> eventTypes;
    int amount;
};



