#include "StartHp.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

void StartHp::notify(EventInfo &info) {
    info.primary->as_entity()->set_health(hp);
}

const std::vector<EventType> StartHp::event_types {CREATED_DONE};

const std::vector<EventType> StartHp::listening_for() const {
    return event_types;
}

StartHp::StartHp(int hp) : hp{hp} {}
