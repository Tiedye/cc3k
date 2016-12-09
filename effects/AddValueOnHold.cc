#include "AddValueOnHold.h"

#include "../event/EventTarget.h"
#include "../entity/Character.h"
#include "../entity/Item.h"

void AddValueOnHold::notify(EventInfo &info) {
    if(info.event_type == ADD_TO_INVENTORY_DONE) {
        info.secondary->as_character()->add_gold(info.primary->as_item()->get_value());
    } else if (info.event_type == REMOVE_FROM_INVENTORY_DONE) {
        info.secondary->as_character()->remove_gold(info.primary->as_item()->get_value());
    }
}

const std::vector<EventType> AddValueOnHold::listening_for() const {
    return {ADD_TO_INVENTORY_DONE, REMOVE_FROM_INVENTORY_DONE};
}
