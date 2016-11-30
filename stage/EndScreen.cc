#include "EndScreen.h"

EndScreen::EndScreen(const std::shared_ptr<State> &state, const int returnId) : Stage(state), returnId{returnId} {}

int EndScreen::run(Game &game) {
    return returnId;
}
