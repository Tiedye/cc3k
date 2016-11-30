#include "HealOnConsume.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

HealOnConsume::HealOnConsume(int amount) : amount(amount) {}

void HealOnConsume::notify(EventInfo &info) {
    info.secondary->asEntity()->heal(amount, info.primary->asEntity());
}

const std::vector<EventType> HealOnConsume::listeningFor() const {
    return eventTypes;
}
const std::vector<EventType> HealOnConsume::eventTypes {CONSUMED_DONE};