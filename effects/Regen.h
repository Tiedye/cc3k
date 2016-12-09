#pragma once

#include "../event/Listener.h"

class Regen : public Listener {
public:
    Regen(int amount);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listening_for() const override;

private:
    static const std::vector<EventType> event_types;
    int amount;
};



