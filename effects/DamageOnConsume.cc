#include "DamageOnConsume.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

DamageOnConsume::DamageOnConsume(int amount) : amount(amount) {}

void DamageOnConsume::notify(EventInfo &info) {
    info.secondary->asEntity()->damage(amount, info.primary->asEntity());
}

const std::vector<EventType> DamageOnConsume::listeningFor() const {
    return eventTypes;
}
const std::vector<EventType> DamageOnConsume::eventTypes {CONSUMED_DONE};