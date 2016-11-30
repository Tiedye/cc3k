#pragma once

#include "../event/Listener.h"
#include <set>

class VenerabilityModifier : public Listener {
public:
    VenerabilityModifier(const int modNumerator, const int modDenominator, const std::vector<int> &to);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listeningFor() const override;

private:
    static const std::vector<EventType> eventTypes;
    const int modNumerator;
    const int modDenominator;
    const std::vector<int> to;
};



