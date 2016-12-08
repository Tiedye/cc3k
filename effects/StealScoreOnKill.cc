#include "StealScoreOnKill.h"

#include "../event/EventInfo.h"
#include "../entity/Character.h"

void StealScoreOnKill::notify(EventInfo &info) {
    auto self = info.primary->asCharacter();
    auto other = info.secondary->asEntity();
    if (other->isDead()) {
        self->addScore(other->currentScore());
    }
}

const std::vector<EventType> StealScoreOnKill::listeningFor() const {
    return {ATTACK_DONE};
}

StealScoreOnKill::StealScoreOnKill():Listener(100) {}
