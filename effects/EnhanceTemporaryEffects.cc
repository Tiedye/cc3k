#include "EnhanceTemporaryEffects.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

EnhanceTemporaryEffects::EnhanceTemporaryEffects(const std::vector<int> &from, const short modNumerator,
                                                 const short modDenominator) : from(from), modNumerator(modNumerator),
                                                                               modDenominator(modDenominator) {}

void EnhanceTemporaryEffects::notify(EventInfo &info) {
    std::shared_ptr<Entity> self {info.primary->asEntity()};
    std::shared_ptr<Entity> other {info.secondary->asEntity()};
    for (int venerableType: from){
        if (other->isA(venerableType)) {
            info.eventDataPointer->short1 = modNumerator;
            info.eventDataPointer->short2 = modDenominator;
            return;
        }
    }
}

const std::vector<EventType> EnhanceTemporaryEffects::listeningFor() const {
    return {TEMP_SET_ADD};
}
