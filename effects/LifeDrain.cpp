#include "LifeDrain.h"

#include "../EventTarget.h"
#include "../entity/Entity.h";

void LifeDrain::notify(EventInfo &info) {
    if (!info.secondaries.empty()) {
        std::shared_ptr<Entity> self {info.primary->asEntity()};
        self->heal(self, amount);
    }
}

std::vector<EventType> LifeDrain::eventTypes {ATTACK_DONE};

const std::vector<EventType> LifeDrain::listeningFor() const {
    return eventTypes;
}

LifeDrain::LifeDrain(int amount): amount{amount} {}
