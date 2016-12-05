#include "Invulnerable.h"

void Invulnerable::notify(EventInfo &info) {
    info.eventDataPointer->integer2 = false;
}

const std::vector<EventType> Invulnerable::listeningFor() const {
    return {ATTACKED};
}
