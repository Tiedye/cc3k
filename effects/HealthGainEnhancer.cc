#include "HealthGainEnhancer.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

void HealthGainEnhancer::notify(EventInfo &info) {
    if (info.secondary->asEntity()->isA(potionId)) {
        info.primary->asEntity()->heal(info.eventInteger * numerator / denominator);
    }
}

const std::vector<EventType> HealthGainEnhancer::listeningFor() const {
    return eventTypes;
}
const std::vector<EventType> HealthGainEnhancer::eventTypes {HEALED_DONE};

HealthGainEnhancer::HealthGainEnhancer(int numerator, int denominator, int potionId) : numerator(numerator),
                                                                                         denominator(denominator),
                                                                                         potionId(potionId) {}
