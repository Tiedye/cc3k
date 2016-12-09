#include "Stair.h"

#include "../stage/Dungeon.h"


void Stair::notify(EventInfo &info) {
    if (info.secondary->as_entity()->is_a(player_id)) dungeon->finish(next_stage);
}

const std::vector<EventType> Stair::listening_for() const {
    return event_types;
}

const std::vector<EventType> Stair::event_types {OCCUPIED_DONE};

Stair::Stair(const int next_stage, const std::shared_ptr<Dungeon> &dungeon, const int player_id) : next_stage(next_stage), dungeon(dungeon), player_id{player_id} {}
