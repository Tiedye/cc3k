#include "WalkAsActionAction.h"

#include "../entity/Character.h"
#include "../stage/Dungeon.h"

WalkAsActionAction::WalkAsActionAction(const std::vector<int> &walkable): Action(MOVE, "Walk", ACTION, EXTERNAL, STRICT_PATH, SQUARE, 0, 0), walkable(walkable) {

}

int WalkAsActionAction::get_range(const Character &source) const {
    return source.get_speed();
}

int WalkAsActionAction::get_min_range(const Character &source) const {
    return 1;
}

bool WalkAsActionAction::can_target(const Position position, const std::shared_ptr<State> &state) {
    CellType cell_type {state->get_current_dungeon()->get_cell_type(position)};
    for(auto type:walkable) {
        if (cell_type == type) return true;
    }
    return false;
}
