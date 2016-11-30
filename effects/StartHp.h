#pragma once


#include "../event/Listener.h"

class StartHp: public Listener {
public:
    StartHp(int hp);

    void notify(EventInfo &info) override;

    static const std::vector<EventType> eventTypes;
    const std::vector<EventType> listeningFor() const override;

private:
    const int hp;
};

