#include "VenerabilityModifier.h"

#include "../entity/Entity.h"
#include "../event/EventTarget.h"

using namespace std;

void VenerabilityModifier::notify(EventInfo &info) {
    for (int venerableTo: to) {
        if (info.secondary->asEntity()->isA(venerableTo)) {
            info.eventDataPointer->integer1 = info.eventDataPointer->integer1 * modNumerator / modDenominator;
            break;
        }
    }
}

const std::vector<EventType> VenerabilityModifier::listeningFor() const {
    return eventTypes;
}

VenerabilityModifier::VenerabilityModifier(const int modNumerator, const int modDenominator, const std::vector<int> &to)
        : modNumerator(modNumerator), modDenominator(modDenominator), to(to) {}

const std::vector<EventType> VenerabilityModifier::eventTypes {ATTACKED};