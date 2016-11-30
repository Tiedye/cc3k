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

map<char, int> SimpleLoader::parseMapping(istream& s) {
    map<char, int> mapping;
    while(!s.eof()) {
        char c;
        string name;
        s >> c >> name;
        mapping.emplace(c, state->loader->getId(name));
    }
    return mapping;
};

std::shared_ptr<Dungeon> SimpleLoader::parseDungeon(const std::map<char, int> &mapping, std::istream &s, int id) {
    s >> noskipws;
    auto dungeon = make_shared<Dungeon>(state, id, 79, 25);
    char c;
    for (int y = 0; y < 25; ++y) {
        for(int x = 0; x < 79; ++y){
            s >> c;
            switch (c) {
                case '|':
                case '-':
                    dungeon->setCellType({y, x}, WALL);
                    break;
                case ' ':
                    dungeon->setCellType({y, x}, EMPTY);
                    break;
                case '.':
                    dungeon->setCellType({y, x}, FLOOR);
                    break;
                case '@':
                    dungeon->setCellType({y, x}, FLOOR);
                    dungeon->spawnPoint = {y, x};
                    break;
                case '+':
                    dungeon->setCellType({y, x}, OPEN_DOOR);
                    break;
                case '#':
                    dungeon->setCellType({y, x}, HALL);
                    break;
                case '\\': {
                    auto stair = make_shared<Entity>();
                    stair->move({y, x});
                    auto stairHandler = make_shared<Stair>(id + 1, dungeon);
                    stair->addListener(stairHandler);
                    dungeon->addEntity(stair);
                    stair->create();
                    break;
                }
                default: {
                    dungeon->setCellType({y, x}, FLOOR);
                    if (mapping.count(c)) {
                        int itemId = mapping.at(c);
                        switch (state->library.getType(itemId)) {
                            case Library::MOB: {
                                auto newMob = state->library.getAMob(itemId);
                                newMob->move({y, x});
                                dungeon->addEntity(newMob);
                                newMob->create();
                                break;
                            }
                            case Library::ITEM: {
                                auto newItem = state->library.getAnItem(itemId);
                                newItem->move({y, x});
                                dungeon->addEntity(newItem);
                                newItem->create();
                                break;
                            }
                            case Library::CONSUMABLE: {
                                auto newConsumable = state->library.getAConsumable(itemId);
                                newConsumable->move({y, x});
                                dungeon->addEntity(newConsumable);
                                newConsumable->create();
                                break;
                            }
                            case Library::EQUIPPABLE: {
                                auto newEquippable = state->library.getAnEquippable(itemId);
                                newEquippable->move({y, x});
                                dungeon->addEntity(newEquippable);
                                newEquippable->create();
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
    ifstream s;
    int nextId = 0;
    s.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        s.open(files[0]);
        map<char, int> mapping = parseMapping(s);
        s.close();
        s.open(files[1]);
        nextId = Game::nextId();
        int count = 0;
        while(!s.eof()) {
            auto dungeon = parseDungeon(mapping, s, Game::getId());
            ostringstream name;
            name << "Floor " << ++count;
            dungeon->setName(name.str());
            game.addStage(1, dungeon);
        }
        game.addStage(1, make_shared<EndScreen>(state, 0)); // returns to the zero state once done
    } catch (const ifstream::failure &e) {
        // TODO, error on no file, or read issue
        cerr << "Error in SimpleLoader reading file" << endl;
        // exit(1);
    }
    s.close();
    return nextId;
}
