#include "State.h"

State::State() : gen(std::random_device()()) {
}

std::shared_ptr<Dungeon> State::getCurrentDungeon() {
    return currentDungeon.lock();
}
