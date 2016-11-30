#include "loader/Loader.h"
#include "State.h"
#include "Game.h"
#include "stage/Menu.h"
#include "stage/EndScreen.h"

#include <iostream>


using namespace std;

int main(int argc, char *argv[]) {
    string gameFile = "data/full_game/game.tdat";
    string dungeonSpec = "data/full_game/dungeon/dungeon.tdat";

    vector<string> dungeonFiles;

    for(int index = 1; index < argc; ++index) {
        string arg {argv[index]};
        if (arg == "--game") {
            ++index;
            if (index == argc) break;
            gameFile = argv[index];
        } else if (arg == "--dungeon_spec") {
            ++index;
            if (index == argc) break;
            dungeonSpec = argv[index];
        } else {
            ++index;
            if (index == argc) break;
            dungeonFiles.push_back(string(argv[index]));
        }
    }

    shared_ptr<State> state;
    shared_ptr<Loader> loader = make_shared<Loader>(state);
    state->loader = loader;
    loader->loadFile(gameFile);

    Game game(state);
    if (!dungeonFiles.empty()) {

    }
}
