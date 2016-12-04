#include "loader/Loader.h"
#include "State.h"
#include "Game.h"
#include "stage/Menu.h"
#include "stage/EndScreen.h"
#include "stage/SimpleLoader.h"
#include "display/ConsoleDungeonIO.h"

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
            if (index == argc) break;
            dungeonFiles.push_back(string(argv[index]));
        }
    }

    shared_ptr<State> state = make_shared<State>();
    shared_ptr<Loader> loader = make_shared<Loader>(state);
    shared_ptr<ConsoleDungeonIO> dungeonIO = make_shared<ConsoleDungeonIO>();
    state->loader = loader;
    state->dungeonRenderer = dungeonIO;
    state->player = make_shared<Character>("Player");
    state->player->makeA(state->loader->getId("player"));
    state->player->controller = dungeonIO;
    state->player->representation = '@';
    loader->loadFile(gameFile);

    Game game(state);
    if (!dungeonFiles.empty()) {
        int menuId, nextId;
        menuId = Game::getId();
        nextId = Game::nextId();
        auto menu = make_shared<Menu>(state, menuId, nextId);
        auto simpleLoader = make_shared<SimpleLoader>(state, dungeonFiles);
        game.addStage(0, menu);
        game.addStage(0, simpleLoader);
        game.setInitialStage(menu->id);
        game.start();
    }
}
