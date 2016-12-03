//
// Created by daniel on 02/12/16.
//

#include "TypeAutoPickup.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"
#include "../entity/Character.h"

void TypeAutoPickup::notify(EventInfo &info) {
    auto to = info.secondary->asCharacter();
    if (to->isA(type)) {
        info.primary->asEntity()->removeFromContainers();
        to->give(info.primary->asItem());
    }
}

const std::vector<EventType> TypeAutoPickup::listeningFor() const {
    return {OCCUPIED_DONE};
}

TypeAutoPickup::TypeAutoPickup(const int type) : type(type) {}
