#include "Regen.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"

using namespace std;

void Regen::notify(EventInfo &info) {
    shared_ptr<Entity> self = info.primary->asEntity();
    self->heal(amount, self);
}

const std::vector<EventType> Regen::listeningFor() const {
    return eventTypes;
}

const vector<EventType> Regen::eventTypes {TURN_START_DONE};

Regen::Regen(int amount) : amount(amount) {}
