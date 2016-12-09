#pragma once


#include "../event/Listener.h"

class StartHp: public Listener {
public:
    StartHp(int hp);

    void notify(EventInfo &info) override;

    static const std::vector<EventType> event_types;
    const std::vector<EventType> listening_for() const override;

private:
    const int hp;
};

