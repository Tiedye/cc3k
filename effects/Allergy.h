#pragma once


#include "../Listener.h"
#include <set>

class Allergy : public Listener {
public:
    Allergy(const int amount, const std::vector<int> &to);

    void notify(EventInfo &info) override;

    static const std::vector<EventType> eventTypes;
    const std::vector<EventType> listeningFor() const override;

private:
    const int amount;
    const std::vector<int> to;
};

