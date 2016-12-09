#pragma once

#include "../event/Listener.h"
class Dungeon;

class Stair : public Listener {
public:
    Stair(const int next_stage, const std::shared_ptr<Dungeon> &dungeon, const int player_id);

    void notify(EventInfo &info) override;

    const std::vector<EventType> listening_for() const override;

private:
    static const std::vector<EventType> event_types;
    const int next_stage;
    const std::shared_ptr<Dungeon> dungeon;
    const int player_id;
};



