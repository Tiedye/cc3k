#include "HealthGainModifier.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

void HealthGainModifier::notify(EventInfo &info) {
    if (info.secondary->as_entity()->is_a(potion_id)) {
        info.event_data_pointer->integer1 = info.event_data_pointer->integer1 * numerator / denominator;
    }
}

const std::vector<EventType> HealthGainModifier::listening_for() const {
    return event_types;
}
const std::vector<EventType> HealthGainModifier::event_types {HEALED};

HealthGainModifier::HealthGainModifier(int numerator, int denominator, int potion_id) : numerator(numerator),
                                                                                         denominator(denominator),
                                                                                         potion_id(potion_id) {}
