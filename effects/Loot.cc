#include "Loot.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"
#include "../entity/Character.h"

Loot::Loot(int amount) : amount(amount) {}

void Loot::notify(EventInfo &info) {
    if(info.secondary->asEntity()->isDead()) {
        info.primary->asCharacter()->addGold(amount);
    }
}

const std::vector<EventType> Loot::eventTypes {ATTACKED_DONE};
const std::vector<EventType> Loot::listeningFor() const {
    return eventTypes;
}
