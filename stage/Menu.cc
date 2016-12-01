#include "Menu.h"
#include "../Game.h"

using namespace std;

int Menu::run(Game &game) {
    state->player->addFeatureSet(*state->library.getRaceSet(state->library.getRaces().begin()->first));
    return next;
}

Menu::Menu(const shared_ptr<State> &state, const int id, const int next) : Stage(state, id), next(next) {}
