#include "EnhanceTemporaryEffects.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

EnhanceTemporaryEffects::EnhanceTemporaryEffects(const short mod_numerator, const short mod_denominator, const std::vector<int> &from) : from(from), mod_numerator(mod_numerator),
                                                                                                                                         mod_denominator(mod_denominator) {}

void EnhanceTemporaryEffects::notify(EventInfo &info) {
    std::shared_ptr<Entity> self {info.primary->as_entity()};
    std::shared_ptr<Entity> other {info.secondary->as_entity()};
    for (int venerable_type: from){
        if (other->is_a(venerable_type)) {
            info.event_data_pointer->short1 = mod_numerator;
            info.event_data_pointer->short2 = mod_denominator;
            return;
        }
    }
}

const std::vector<EventType> EnhanceTemporaryEffects::listening_for() const {
    return {TEMP_SET_ADD};
}
