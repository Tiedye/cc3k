#include "SimpleLoader.h"

#include "Dungeon.h"

#include "../loader/Loader.h"
#include "../Game.h"
#include "../loader/Stair.h"
#include "EndScreen.h"

#include <fstream>
#include <sstream>

using namespace std;

SimpleLoader::SimpleLoader(const std::shared_ptr<State> &state, const std::vector<std::string> &files) : Stage(state), files(files) {}

map<char, int> SimpleLoader::parse_mapping(istream& s) {
    map<char, int> mapping;
    char c;
    string name;
    while(s >> c >> name) {
        mapping.emplace(c, state->loader->get_id(name));
    }
    return mapping;
};

std::shared_ptr<Dungeon> SimpleLoader::parse_dungeon(const std::map<char, int> &mapping, std::istream &s) {
    s >> noskipws;
    auto dungeon = make_shared<Dungeon>(state, Game::get_id(), 79, 25);
    char c;
    for (int y = 0; y < 25; ++y) {
        for(int x = 0; x < 79; ++x) {
            s >> c;
            switch (c) {
                case '|':
                case '-':
                    dungeon->initialize_cell({y, x}, WALL);
                    break;
                case ' ':
                    dungeon->initialize_cell({y, x}, EMPTY);
                    break;
                case '.':
                    dungeon->initialize_cell({y, x}, FLOOR);
                    break;
                case '@':
                    dungeon->initialize_cell({y, x}, FLOOR);
                    dungeon->spawn_point = {y, x};
                    break;
                case '+':
                    dungeon->initialize_cell({y, x}, OPEN_DOOR);
                    break;
                case '#':
                    dungeon->initialize_cell({y, x}, HALL);
                    break;
                case '\\': {
                    dungeon->initialize_cell({y, x}, FLOOR);
                    auto stair = make_shared<Entity>();
                    stair->representation = '\\';
                    stair->move({y, x});
                    auto stair_handler = make_shared<Stair>(Game::next_id(), dungeon, state->loader->get_id("player"));
                    stair->add_listener(stair_handler);
                    dungeon->initialize_entity(stair);
                    stair->create();
                    dungeon->default_next = Game::next_id();
                    break;
                }
                default: {
                    dungeon->initialize_cell({y, x}, FLOOR);
                    if (mapping.count(c)) {
                        int item_id = mapping.at(c);
                        switch (state->library.get_type(item_id)) {
                            case Library::MOB: {
                                auto new_mob = state->library.get_a_mob(item_id);
                                new_mob->move({y, x});
                                dungeon->initialize_entity(new_mob);
                                new_mob->create();
                                break;
                            }
                            case Library::ITEM: {
                                auto new_item = state->library.get_an_item(item_id);
                                new_item->move({y, x});
                                dungeon->initialize_entity(new_item);
                                new_item->create();
                                break;
                            }
                            case Library::CONSUMABLE: {
                                auto new_consumable = state->library.get_a_consumable(item_id);
                                new_consumable->move({y, x});
                                dungeon->initialize_entity(new_consumable);
                                new_consumable->create();
                                break;
                            }
                            case Library::EQUIPPABLE: {
                                auto new_equippable = state->library.get_an_equippable(item_id);
                                new_equippable->move({y, x});
                                dungeon->initialize_entity(new_equippable);
                                new_equippable->create();
                                break;
                            }
                            case Library::RACE:break;
                        }
                    }
                    break;
                }
            }
        }
        s >> c; // drain newline
    }
    return dungeon;
}

int SimpleLoader::run(Game &game) {
    game.remove_states(1);
    ifstream s;
    int next_id = 0;
    s.open(files[0]); // TODO, check and err on file open failure
    map<char, int> mapping = parse_mapping(s);
    s.close();
    s.open(files[1]);
    next_id = Game::next_id();
    int count = 0;
    while(s.peek(), !s.eof()) {
        auto dungeon = parse_dungeon(mapping, s);
        ostringstream name;
        name << "Floor " << ++count;
        dungeon->set_name(name.str());
        game.add_stage(1, dungeon);
    }
    game.add_stage(1, make_shared<EndScreen>(state, 0)); // returns to the zero state once done
    s.close();
    return next_id;
}
