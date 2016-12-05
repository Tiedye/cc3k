#include "WalkAction.h"

#include "../stage/Dungeon.h"
#include "../entity/Character.h"

int WalkAction::getRange(Character &source) const {
    return source.getSpeed();
}

int WalkAction::getMinRange(Character &source) const {
    return 1;
}

WalkAction::WalkAction(const std::vector<int> &walkable) :Action(MOVE, MOVEACTION, SELF, STRICT_PATH), walkable(walkable) {}

bool WalkAction::canTarget(const Position position, const std::shared_ptr<State> &state) {
    CellType cellType {state->currentDungeon->getCellType(position)};
    for(auto type:walkable) {
        if (cellType == type) return true;
    }
    return false;
}
