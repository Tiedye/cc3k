#include "Invulnerable.h"

void Invulnerable::notify(EventInfo &info) {
    info.event_data_pointer->integer2 = false;
}

const std::vector<EventType> Invulnerable::listening_for() const {
    return {ATTACKED};
}
