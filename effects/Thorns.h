#pragma once

#include "../event/Listener.h"

class Thorns : public Listener {
public:
    Thorns(const int numerator, const int denominator);
    void notify(EventInfo &info) override;
    const std::vector<EventType> listening_for() const override;
private:
    const int numerator;
    const int denominator;
};
