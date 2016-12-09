#pragma once


#include "../event/Listener.h"

class TypeAutoPickup : public Listener{
public:
    void notify(EventInfo &info) override;

    const std::vector<EventType> listening_for() const override;

private:
    const int type;
public:
    TypeAutoPickup(const int type);
};

