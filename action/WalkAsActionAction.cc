#include "WalkAsActionAction.h"

#include "../entity/Character.h"
#include "../stage/Dungeon.h"

WalkAsActionAction::WalkAsActionAction(const std::vector<int> &walkable):Action(MOVE, ACTION, SELF, STRICT_PATH), walkable(walkable) {

}

int WalkAsActionAction::getRange(Character &source) const {
    return source.getSpeed();
}

int WalkAsActionAction::getMinRange(Character &source) const {
    return 1;
}

bool WalkAsActionAction::canTarget(const Position position, const std::shared_ptr<State> &state) {
    CellType cellType {state->currentDungeon->getCellType(position)};
    for(auto type:walkable) {
        if (cellType == type) return true;
    }
    return false;
}
