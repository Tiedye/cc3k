#pragma once

#include "../event/Listener.h"

class DamageTakenEnhancer : public Listener {
public:
    DamageTakenEnhancer(int numerator, int denominator, int potionId);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listeningFor() const override;

private:
    static const std::vector<EventType> eventTypes;

    int numerator;
    int denominator;
    int potionId;
};



