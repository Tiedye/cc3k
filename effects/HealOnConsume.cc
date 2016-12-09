#include "HealOnConsume.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

HealOnConsume::HealOnConsume(int amount) : amount(amount) {}

void HealOnConsume::notify(EventInfo &info) {
    info.secondary->as_entity()->heal(amount, info.primary->as_entity());
}

const std::vector<EventType> HealOnConsume::listening_for() const {
    return event_types;
}
const std::vector<EventType> HealOnConsume::event_types {CONSUMED_DONE};