#pragma once

#include "../event/Listener.h"

class EnhanceTemporaryEffects : public Listener {
public:
    EnhanceTemporaryEffects(const short mod_numerator, const short mod_denominator, const std::vector<int> &from);

private:
    void notify(EventInfo &info) override;

    const std::vector<EventType> listening_for() const override;

private:
    const std::vector<int> from;
    const short mod_numerator;
    const short mod_denominator;
};
