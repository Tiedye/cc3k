#include "Stage.h"

#include "../Game.h"

Stage::Stage(const std::shared_ptr<State> &state, int id) : id{id == -1 ? Game::get_id() : id}, state(state) {

}
