#include "DamageTakenModifier.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

void DamageTakenModifier::notify(EventInfo &info) {
    if (info.secondary->as_entity()->is_a(potion_id)) {
        info.event_data_pointer->integer1 = info.event_data_pointer->integer1 * numerator / denominator;
    }
}

const std::vector<EventType> DamageTakenModifier::event_types {ATTACKED};

const std::vector<EventType> DamageTakenModifier::listening_for() const {
    return event_types;
}

DamageTakenModifier::DamageTakenModifier(int numerator, int denominator, int potion_id) : numerator(numerator),
                                                                                           denominator(denominator),
                                                                                           potion_id(potion_id) {}
