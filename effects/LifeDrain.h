#pragma once


#include "../Listener.h"

class LifeDrain : public Listener {
public:
    LifeDrain(int amount);

    void notify(EventInfo &info) override;

    static const std::vector<EventType> eventTypes;
    const std::vector<EventType> listeningFor() const override;

private:
    const int amount;
};

