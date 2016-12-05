#pragma once


#include "../event/Listener.h"

class Invulnerable : public Listener {
public:
    void notify(EventInfo &info) override;
    const std::vector<EventType> listeningFor() const override;
};


