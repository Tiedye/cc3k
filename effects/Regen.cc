#include "Regen.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

using namespace std;

void Regen::notify(EventInfo &info) {
    shared_ptr<Entity> self = info.primary->as_entity();
    self->heal(amount, self);
}

const std::vector<EventType> Regen::listening_for() const {
    return event_types;
}

const vector<EventType> Regen::event_types {TURN_START_DONE};

Regen::Regen(int amount) : amount(amount) {}
