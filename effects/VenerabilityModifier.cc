#include "VenerabilityModifier.h"

#include "../entity/Entity.h"
#include "../event/EventTarget.h"

using namespace std;

void VenerabilityModifier::notify(EventInfo &info) {
    for (int venerable_to: to) {
        if (info.secondary->as_entity()->is_a(venerable_to)) {
            info.event_data_pointer->integer1 = info.event_data_pointer->integer1 * mod_numerator / mod_denominator;
            break;
        }
    }
}

const std::vector<EventType> VenerabilityModifier::listening_for() const {
    return event_types;
}

VenerabilityModifier::VenerabilityModifier(const int mod_numerator, const int mod_denominator, const std::vector<int> &to)
        : mod_numerator(mod_numerator), mod_denominator(mod_denominator), to(to) {}

const std::vector<EventType> VenerabilityModifier::event_types {ATTACKED};