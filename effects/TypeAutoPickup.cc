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
    auto to = info.secondary->as_character();
    if (to->is_a(type)) {
        auto self = info.primary->as_item();
        EventInfo::Data data;
        data.integer1 = true;
        self->trigger(ADD_TO_INVENTORY, data, to);
        if (data.integer1) {
            self->remove_from_containers();
            to->give(info.primary->as_item());
            self->trigger(ADD_TO_INVENTORY_DONE, to);
        }
        info.primary->as_entity()->remove_from_containers();
        to->give(info.primary->as_item());
    }
}

const std::vector<EventType> TypeAutoPickup::listening_for() const {
    return {OCCUPIED_DONE};
}

TypeAutoPickup::TypeAutoPickup(const int type) : type(type) {}
