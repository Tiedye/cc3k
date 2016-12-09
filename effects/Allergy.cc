#include <set>
#include "Allergy.h"
#include "../event/EventInfo.h"
#include "../entity/Entity.h"

using namespace std;

void Allergy::notify(EventInfo &info) {
    std::shared_ptr<Entity> self {info.primary->as_entity()};
    std::shared_ptr<Entity> other {info.secondary->as_entity()};
    for (int allergy: to){
        if (other->is_a(allergy)) {
            self->damage(amount, other);
            return;
        }
    }
}

const std::vector<EventType> Allergy::listening_for() const {
    return event_types;
}

Allergy::Allergy(const int amount, const std::vector<int> &to) : amount(amount), to(to) {}

const std::vector<EventType> Allergy::event_types {ATTACK_DONE};