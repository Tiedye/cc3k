#include "StartHp.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

void StartHp::notify(EventInfo &info) {
    info.primary->asEntity()->setHealth(hp);
}

const std::vector<EventType> StartHp::eventTypes {CREATED_DONE};

const std::vector<EventType> StartHp::listeningFor() const {
    return eventTypes;
}

StartHp::StartHp(int hp) : hp{hp} {}
