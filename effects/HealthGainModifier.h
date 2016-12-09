#pragma once

#include "../event/Listener.h"

class HealthGainModifier : public Listener {
public:
    HealthGainModifier(int numerator, int denominator, int potion_id);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listening_for() const override;

private:
    static const std::vector<EventType> event_types;

    int numerator;
    int denominator;
    int potion_id;
};



