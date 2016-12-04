#pragma once

#include "../event/Listener.h"

class EnhanceTemporaryEffects : public Listener {
public:
    EnhanceTemporaryEffects(const std::vector<int> &from, const short modNumerator, const short modDenominator);

private:
    void notify(EventInfo &info) override;

    const std::vector<EventType> listeningFor() const override;

private:
    const std::vector<int> from;
    const short modNumerator;
    const short modDenominator;
};
