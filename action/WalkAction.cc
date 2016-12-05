#include "WalkAction.h"

#include "../stage/Dungeon.h"
#include "../entity/Character.h"

int WalkAction::getRange(const Character &source) const {
    return source.getSpeed();
}

int WalkAction::getMinRange(const Character &source) const {
    return 1;
}

WalkAction::WalkAction(const std::vector<int> &walkable) : Action(MOVE, "Walk", MOVEACTION, EXTERNAL, STRICT_PATH, SQUARE, 0, 0), walkable(walkable) {}

bool WalkAction::canTarget(const Position position, const std::shared_ptr<State> &state) {
    CellType cellType {state->getCurrentDungeon()->getCellType(position)};
    for(auto type:walkable) {
        if (cellType == type) return true;
    }
    return false;
}
