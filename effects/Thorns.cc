#include "Thorns.h"

#include "../event/EventInfo.h"
#include "../entity/Entity.h"

void Thorns::notify(EventInfo &info) {
    info.secondary->as_entity()->damage(info.event_integer * numerator / denominator);
}

const std::vector<EventType> Thorns::listening_for() const {
    return {ATTACKED_DONE};
}

Thorns::Thorns(const int numerator, const int denominator) : numerator(numerator), denominator(denominator) {}
