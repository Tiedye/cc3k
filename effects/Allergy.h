#pragma once


#include "../event/Listener.h"
#include <set>

class Allergy : public Listener {
public:
    Allergy(const int amount, const std::vector<int> &to);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listening_for() const override;

private:
    static const std::vector<EventType> event_types;
    const int amount;
    const std::vector<int> to;
};

