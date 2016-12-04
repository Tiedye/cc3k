#include <set>
#include "Allergy.h"
#include "../event/EventInfo.h"
#include "../entity/Entity.h"

using namespace std;

void Allergy::notify(EventInfo &info) {
    std::shared_ptr<Entity> self {info.primary->asEntity()};
    std::shared_ptr<Entity> other {info.secondary->asEntity()};
    for (int allergy: to){
        if (other->isA(allergy)) {
            self->damage(amount, other);
            return;
        }
    }
}

const std::vector<EventType> Allergy::listeningFor() const {
    return eventTypes;
}

Allergy::Allergy(const int amount, const std::vector<int> &to) : amount(amount), to(to) {}

const std::vector<EventType> Allergy::eventTypes {ATTACK_DONE};