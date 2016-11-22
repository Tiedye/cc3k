#include <set>
#include "Allergy.h"
#include "../EventInfo.h"
#include "../entity/Entity.h"

void Allergy::notify(EventInfo &info) {
    Entity *self = info.primary->asEntity();
    for (auto target:info.secondaries) {
        Entity *other = target->asEntity();
        for (int allergy: to){
            if (other->isA(allergy)) {
                self->damage(other, amount);
                return;
            }
        }
    }
}

const std::vector<EventType> Allergy::listeningFor() const {
    return nullptr;
}

Allergy::Allergy(const int amount, const std::vector<int> &to) : amount(amount), to(to) {}

std::vector<EventType> Allergy::eventTypes {ATTACK_DONE};