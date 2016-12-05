#include "StateSetOnAttacked.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"
#include "../State.h"

void StateSetOnAttacked::notify(EventInfo &info) {
    if(info.secondary->asEntity()->isA(whenAttackedBy)) {
        state.lock()->aiGetInteger(idToSet) = true;
    }
}

const std::vector<EventType> StateSetOnAttacked::listeningFor() const {
    return {ATTACKED_DONE};
}

StateSetOnAttacked::StateSetOnAttacked(const int idToSet, const int whenAttackedBy, const std::weak_ptr<State> &state) : idToSet(idToSet), whenAttackedBy(whenAttackedBy),
                                                                                                                         state(state) {}
