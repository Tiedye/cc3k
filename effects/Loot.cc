#include "Loot.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"
#include "../entity/Character.h"

Loot::Loot(int amount) : amount(amount) {}

void Loot::notify(EventInfo &info) {
    if(info.secondary->as_entity()->is_dead()) {
        info.primary->as_character()->add_gold(amount);
    }
}

const std::vector<EventType> Loot::event_types {ATTACKED_DONE};
const std::vector<EventType> Loot::listening_for() const {
    return event_types;
}
