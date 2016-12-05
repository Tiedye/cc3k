//
// Created by daniel on 02/12/16.
//

#include "TypeAutoPickup.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"
#include "../entity/Character.h"
#include "../entity/Item.h"
#include "../display/ConsoleDungeonIO.h"

using namespace std;

void TypeAutoPickup::notify(EventInfo &info) {
    auto to = info.secondary->asCharacter();
    if (to->isA(type)) {
        auto self = info.primary->asItem();
        EventInfo::Data data;
        data.integer1 = true;
        self->trigger(ADD_TO_INVENTORY, data, to);
        if (data.integer1) {
            self->removeFromContainers();
            to->give(info.primary->asItem());
            self->trigger(ADD_TO_INVENTORY_DONE, to);
        }
        info.primary->asEntity()->removeFromContainers();
        to->give(info.primary->asItem());
    }
}

const std::vector<EventType> TypeAutoPickup::listeningFor() const {
    return {OCCUPIED_DONE};
}

TypeAutoPickup::TypeAutoPickup(const int type) : type(type) {}
