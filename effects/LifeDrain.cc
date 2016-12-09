#include "LifeDrain.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

void LifeDrain::notify(EventInfo &info) {
    std::shared_ptr<Entity> self {info.primary->as_entity()};
    self->heal(amount, self);
}

const std::vector<EventType> LifeDrain::event_types {ATTACK_DONE};

const std::vector<EventType> LifeDrain::listening_for() const {
    return event_types;
}

LifeDrain::LifeDrain(int amount): amount{amount} {}
