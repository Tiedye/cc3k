#include "WalkAsActionAction.h"

#include "../entity/Character.h"
#include "../stage/Dungeon.h"

WalkAsActionAction::WalkAsActionAction(const std::vector<int> &walkable): Action(MOVE, "Walk", ACTION, EXTERNAL, STRICT_PATH, SQUARE, 0, 0), walkable(walkable) {

}

int WalkAsActionAction::getRange(const Character &source) const {
    return source.getSpeed();
}

int WalkAsActionAction::getMinRange(const Character &source) const {
    return 1;
}

bool WalkAsActionAction::canTarget(const Position position, const std::shared_ptr<State> &state) {
    CellType cellType {state->getCurrentDungeon()->getCellType(position)};
    for(auto type:walkable) {
        if (cellType == type) return true;
    }
    return false;
}
