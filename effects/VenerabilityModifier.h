#pragma once

#include "../event/Listener.h"
#include <set>

class VenerabilityModifier : public Listener {
public:
    VenerabilityModifier(const int mod_numerator, const int mod_denominator, const std::vector<int> &to);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listening_for() const override;

private:
    static const std::vector<EventType> event_types;
    const int mod_numerator;
    const int mod_denominator;
    const std::vector<int> to;
};



