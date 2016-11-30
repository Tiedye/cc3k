#include "Stair.h"

#include "../stage/Dungeon.h"


void Stair::notify(EventInfo &info) {
    dungeon->finish(nextStage);
}

const std::vector<EventType> Stair::listeningFor() const {
    return eventTypes;
}

const std::vector<EventType> Stair::eventTypes {OCCUPIED_DONE};

Stair::Stair(const int nextStage, const std::shared_ptr<Dungeon> &dungeon) : nextStage(nextStage), dungeon(dungeon) {}
