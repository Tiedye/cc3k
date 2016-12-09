#pragma once


#include "../event/Listener.h"

class LifeDrain : public Listener {
public:
    LifeDrain(int amount);

    void notify(EventInfo &info) override;

    static const std::vector<EventType> event_types;
    const std::vector<EventType> listening_for() const override;

private:
    const int amount;
};

