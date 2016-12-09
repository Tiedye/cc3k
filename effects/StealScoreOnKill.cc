#include "StealScoreOnKill.h"

#include "../event/EventInfo.h"
#include "../entity/Character.h"

void StealScoreOnKill::notify(EventInfo &info) {
    auto self = info.primary->as_character();
    auto other = info.secondary->as_entity();
    if (other->is_dead()) {
        self->add_score(other->current_score());
    }
}

const std::vector<EventType> StealScoreOnKill::listening_for() const {
    return {ATTACK_DONE};
}

StealScoreOnKill::StealScoreOnKill():Listener(100) {}
