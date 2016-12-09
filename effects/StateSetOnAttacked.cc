#include "StateSetOnAttacked.h"

#include "../event/EventTarget.h"
#include "../entity/Entity.h"
#include "../State.h"

void StateSetOnAttacked::notify(EventInfo &info) {
    if(info.secondary->as_entity()->is_a(when_attacked_by)) {
        state.lock()->ai_get_integer(id_to_set) = true;
    }
}

const std::vector<EventType> StateSetOnAttacked::listening_for() const {
    return {ATTACKED_DONE};
}

StateSetOnAttacked::StateSetOnAttacked(const int id_to_set, const int when_attacked_by, const std::weak_ptr<State> &state) : id_to_set(id_to_set), when_attacked_by(when_attacked_by),
                                                                                                                         state(state) {}
