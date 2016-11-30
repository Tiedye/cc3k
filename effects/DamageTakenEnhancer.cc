#include "DamageTakenEnhancer.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

void DamageTakenEnhancer::notify(EventInfo &info) {
    if (info.secondary->asEntity()->isA(potionId)) {
        info.primary->asEntity()->heal(info.eventInteger * numerator / denominator);
    }
}

const std::vector<EventType> DamageTakenEnhancer::eventTypes {ATTACKED_DONE};

const std::vector<EventType> DamageTakenEnhancer::listeningFor() const {
    return eventTypes;
}

DamageTakenEnhancer::DamageTakenEnhancer(int numerator, int denominator, int potionId) : numerator(numerator),
                                                                                           denominator(denominator),
                                                                                           potionId(potionId) {}
