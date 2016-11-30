#pragma once


#include "../event/Listener.h"
#include <set>

class Allergy : public Listener {
public:
    Allergy(const int amount, const std::vector<int> &to);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listeningFor() const override;

private:
    static const std::vector<EventType> eventTypes;
    const int amount;
    const std::vector<int> to;
};

