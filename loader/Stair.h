#pragma once

#include "../event/Listener.h"
class Dungeon;

class Stair : public Listener {
public:
    Stair(const int nextStage, const std::shared_ptr<Dungeon> &dungeon);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listeningFor() const override;

private:
    static const std::vector<EventType> eventTypes;
    const int nextStage;
    const std::shared_ptr<Dungeon> dungeon;
};



