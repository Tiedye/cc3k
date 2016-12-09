#include "State.h"

State::State() : gen(std::random_device()()) {
}

std::shared_ptr<Dungeon> State::get_current_dungeon() {
    return current_dungeon.lock();
}
