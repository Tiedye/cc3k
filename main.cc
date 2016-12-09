#include "loader/Loader.h"
#include "State.h"
#include "Game.h"
#include "stage/Menu.h"
#include "stage/EndScreen.h"
#include "stage/SimpleLoader.h"
#include "display/ConsoleDungeonIO.h"
#include "stage/RandomLevelLoader.h"

#include <iostream>


using namespace std;

int main(int argc, char *argv[]) {
    string game_file = "data/full_game/game.tdat";
    string dungeon_spec = "data/full_game/dungeon/dungeon.tdat";

    vector<string> dungeon_files;

    for(int index = 1; index < argc; ++index) {
        string arg {argv[index]};
        if (arg == "--game") {
            ++index;
            if (index == argc) break;
            game_file = argv[index];
        } else if (arg == "--dungeon_spec") {
            ++index;
            if (index == argc) break;
            dungeon_spec = argv[index];
        } else {
            if (index == argc) break;
            dungeon_files.push_back(string(argv[index]));
        }
    }

    shared_ptr<State> state = make_shared<State>();
    shared_ptr<Loader> loader = make_shared<Loader>(state);
    shared_ptr<ConsoleDungeonIO> dungeon_i_o = make_shared<ConsoleDungeonIO>();
    state->loader = loader;
    state->dungeon_renderer = dungeon_i_o;
    state->player = make_shared<Character>("Player");
    state->player->make_a(state->loader->get_id("player"));
    state->player->controller = dungeon_i_o;
    state->player->representation = '@';
    loader->load_file(game_file);

    Game game(state);
    if (!dungeon_files.empty()) {
        auto menu = make_shared<Menu>(state);
        auto simple_loader = make_shared<SimpleLoader>(state, dungeon_files);
        game.add_stage(0, menu);
        game.add_stage(0, simple_loader);
        game.set_initial_stage(menu->id);
        game.add_stage(1, make_shared<EndScreen>(state, menu->id));
    } else {
        auto menu = make_shared<Menu>(state);
        auto random_loader = make_shared<RandomLevelLoader>(state, dungeon_spec);
        game.add_stage(0, menu);
        game.add_stage(0, random_loader);
        game.set_initial_stage(menu->id);
    }
    game.start();
}
