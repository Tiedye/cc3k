#pragma once


#include "../event/Listener.h"

class AddValueOnHold : public Listener {
public:
    void notify(EventInfo &info) override;
    const std::vector<EventType> listening_for() const override;
};


