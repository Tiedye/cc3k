#include "AddValueOnHold.h"

#include "../event/EventTarget.h"
#include "../entity/Character.h"
#include "../entity/Item.h"

void AddValueOnHold::notify(EventInfo &info) {
    if(info.eventType == ADD_TO_INVENTORY_DONE) {
        info.secondary->asCharacter()->addGold(info.primary->asItem()->getValue());
    } else if (info.eventType == REMOVE_FROM_INVENTORY_DONE) {
        info.secondary->asCharacter()->removeGold(info.primary->asItem()->getValue());
    }
}

const std::vector<EventType> AddValueOnHold::listeningFor() const {
    return {ADD_TO_INVENTORY_DONE, REMOVE_FROM_INVENTORY_DONE};
}
