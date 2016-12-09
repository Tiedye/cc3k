#include "DamageOnConsume.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

DamageOnConsume::DamageOnConsume(int amount) : amount(amount) {}

void DamageOnConsume::notify(EventInfo &info) {
    info.secondary->as_entity()->damage(amount, info.primary->as_entity());
}

const std::vector<EventType> DamageOnConsume::listening_for() const {
    return event_types;
}
const std::vector<EventType> DamageOnConsume::event_types {CONSUMED_DONE};