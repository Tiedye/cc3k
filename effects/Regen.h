#pragma once

#include "../event/Listener.h"

class Regen : public Listener {
public:
    Regen(int amount);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listeningFor() const override;

private:
    static const std::vector<EventType> eventTypes;
    int amount;
};


