#include <fstream>
#include <iostream>
#include <queue>
#include "RandomLevelLoader.h"
#include "../loader/Loader.h"
#include "../Game.h"
#include "../stage/Dungeon.h"
#include "../loader/Stair.h"
#include "EndScreen.h"
#include "../util/Position.h"

using namespace std;

RandomLevelLoader::RandomLevelLoader(const std::shared_ptr<State> &state, const std::string &configFile) : Stage(state), configFile(configFile) {}

void RandomLevelLoader::SpawnSet::addGroup(int weight, const std::vector<int> &items) {
    totalWeight += weight;
    groups.emplace(weight, items);
}

std::vector<int> RandomLevelLoader::SpawnSet::getGroup(int selection) {
    for (auto &group:groups) {
        if (selection < group.first) {
            return group.second;
        }
        selection -= group.first;
    }
    return groups.begin()->second;
}

int RandomLevelLoader::SpawnSet::getTotalWeight() const {
    return totalWeight;
}

RandomLevelLoader::SpawnSet::SpawnSet() {}

int RandomLevelLoader::run(Game &game) {
    ifstream s{configFile};
    if (s) {
        int initialDungeon {-1};

        string command;
        while (s >> command) {
            if (command == "start") {
                string object;
                s >> object;
                if (object == "probabilitySet") {
                    parseProbabilitySet(s);
                } else if (object == "dungeon") {
                    parseDungeon(s);
                } else {
                    cerr << "Unrecognized command \"" << command << "\"" << endl;
                }
            } else if (command == "initial") {
                string dungeon;
                s >> dungeon;
                initialDungeon = state->loader->parseId(dungeon);
            }
        }

        // make dungeons

        for (auto &d:dungeonObjects) {
            idMapping.emplace(d.second.id, Game::getId());
        }

        for (auto &d:dungeonObjects) {
            auto dungeon = genDungeon(d.second);
            game.addStage(1, dungeon);
        }
        game.addStage(1, make_shared<EndScreen>(state, 0));
        return idMapping[initialDungeon];

    } else {
        cerr << "Could not load level file \"" << configFile << "\"" << endl;
        return -1;
    }
}

void RandomLevelLoader::parseProbabilitySet(std::istream &s) {
    ProbabilitySet probabilitySet;
    string name;
    s >> name;
    int id{state->loader->parseId(name)};
    string command;
    while (s >> command, command != "end") {
        if (command == "param") {
            string param;
            int value;
            s >> param >> value;
            if (param == "cellHeightMean") { probabilitySet.chm = value; }
            else if (param == "cellHeightShift") { probabilitySet.chs = value; }
            else if (param == "cellWidthMean") { probabilitySet.cwm = value; }
            else if (param == "cellWidthShift") { probabilitySet.cws = value; }
            else if (param == "cellCountMean") { probabilitySet.ccm = value; }
            else if (param == "cellCountShift") { probabilitySet.ccs = value; }
            else if (param == "verticalHallLengthMean") { probabilitySet.vhlm = value; }
            else if (param == "verticalHallLengthShift") { probabilitySet.vhls = value; }
            else if (param == "horizontalHallLengthMean") { probabilitySet.hhlm = value; }
            else if (param == "horizontalHallLengthShift") { probabilitySet.hhls = value; }
        } else if (command == "spawnSet") {
            probabilitySet.spawnSets.push_back(parseSpawnSet(s));
        } else {
            cerr << "Unknown Command \"" << command << "\" when parsing probability set" << endl;
        }
    }
    probabilitySets.emplace(id, move(probabilitySet));
}

void RandomLevelLoader::parseDungeon(std::istream &s) {
    DungeonObject dungeonObject;
    string dungeonName;
    s >> dungeonName;
    int id{state->loader->parseId(dungeonName)};
    dungeonObject.id = id;
    string command;
    while (s >> command, command != "end") {
        if (command == "set") {
            string name;
            s >> name;
            dungeonObject.probabilitySet = probabilitySets.at(state->loader->parseId(name));
        } else if (command == "next") {
            string name;
            s >> name;
            dungeonObject.next = state->loader->parseId(name);
        } else if (command == "size") {
            int height, width;
            s >> height >> width;
            dungeonObject.height = height;
            dungeonObject.width = width;
        } else {
            cerr << "Unknown Command \"" << command << "\" when parsing dungeon object" << endl;
        }
    }
    dungeonObjects.emplace(id, move(dungeonObject));
}

RandomLevelLoader::SpawnSet RandomLevelLoader::parseSpawnSet(std::istream &s) {
    SpawnSet spawnSet;
    int amount;
    s >> amount;
    spawnSet.amount = amount;
    string token;
    int weight {0};
    vector<int> current;
    while (s >> token, token != "done") {
        if (token == "[") {
            s >> weight;
        } else if (token == "]") {
            spawnSet.addGroup(weight, current);
            current.clear();
            weight = -1;
        } else {
            if (weight == -1) {
                cerr << "Must start spawn group with \"[\" not " << token << endl;
            } else {
                current.push_back(state->loader->parseId(token));
            }
        }
    }
    return spawnSet;
}

bool onField(const Position position, const int height, const int width) {
    return position.x >= 0 && position.x < width && position.y >= 0 && position.y < height;
}

Position around[]{
        {1, 1},
        {1, 0},
        {1, -1},
        {0, 1},
        {0, -1},
        {-1, 1},
        {-1, 0},
        {-1, -1}
};

Position adjacent[]{
        //{1, 1},
        {1,  0},
        //{1, -1},
        //{-1, 1},
        {-1, 0},
        //{-1, -1},
        {0,  1},
        {0,  -1}
};

std::shared_ptr<Dungeon> RandomLevelLoader::genDungeon(const DungeonObject &dungeonObject) {
    vector<CellType> cells(dungeonObject.width * dungeonObject.height, EMPTY);
    vector<int> rooms(dungeonObject.width * dungeonObject.height, -1);

    poisson_distribution<> h{dungeonObject.probabilitySet.chm};
    poisson_distribution<> w{dungeonObject.probabilitySet.cwm};

    poisson_distribution<> c{dungeonObject.probabilitySet.ccm};

    uniform_int_distribution<> all;
    auto &gen = state->gen;

    /* gen rooms */

    int cc = c(gen) + dungeonObject.probabilitySet.ccs;
    for (int cn{0}; cn < cc; ++cn) {
        int ch = h(gen) + dungeonObject.probabilitySet.chs;
        int cw = w(gen) + dungeonObject.probabilitySet.cws;
        int y = all(gen) % (dungeonObject.height - 2 - ch) + 1;
        int x = all(gen) % (dungeonObject.width - 2 - cw) + 1;

        for (int yv{y}; yv < y + ch; ++yv) {
            for (int xv{x}; xv < x + cw; ++xv) {
                cells[yv * dungeonObject.width + xv] = FLOOR;
            }
        }
    }

    /* !gen rooms */
    /* gen halls */

    vector<vector<Position>> roomContents;

    for (int y{0}; y < dungeonObject.height; ++y) {
        for (int x{0}; x < dungeonObject.width; ++x) {
            rooms[y * dungeonObject.width + x] = -1;
        }
    }

    int roomIds{0};
    for (int y{0}; y < dungeonObject.height; ++y) {
        for (int x{0}; x < dungeonObject.width; ++x) {
            if (cells[y * dungeonObject.width + x] == FLOOR && rooms[y * dungeonObject.width + x] == -1) {
                int roomId = roomIds++;
                vector<Position> contents;
                queue<Position> toFill;
                toFill.push({y, x});
                rooms[y * dungeonObject.width + x] = roomId;
                while (!toFill.empty()) {
                    Position currentPosition = toFill.front();
                    contents.push_back(currentPosition);
                    toFill.pop();
                    for (Position &delta:around) {
                        auto pos = delta + currentPosition;
                        if (onField(pos, dungeonObject.height, dungeonObject.width) && rooms[pos.y * dungeonObject.width + pos.x] == -1 &&
                            cells[pos.y * dungeonObject.width + pos.x] == FLOOR) {
                            rooms[pos.y * dungeonObject.width + pos.x] = roomId;
                            toFill.push(pos);
                        }
                    }
                }
                roomContents.push_back(move(contents));
                break;
            }
        }
    }

    poisson_distribution<> vhld{dungeonObject.probabilitySet.vhlm};
    poisson_distribution<> hhld{dungeonObject.probabilitySet.hhlm};

    for (auto &room:roomContents) {
        auto pos = room[uniform_int_distribution<>(0, room.size() - 1)(gen)];
        int vhl = vhld(gen) + dungeonObject.probabilitySet.vhls;
        int end = pos.y + vhl;
        for (; pos.y != end; ++pos.y) {
            if (onField(pos, dungeonObject.height, dungeonObject.width) && cells[pos.y * dungeonObject.width + pos.x] == EMPTY) {
                cells[pos.y * dungeonObject.width + pos.x] = HALL;
            }
        }
        vhl = vhld(gen) + dungeonObject.probabilitySet.vhls;
        end = pos.y - vhl;
        for (; pos.y != end; --pos.y) {
            if (onField(pos, dungeonObject.height, dungeonObject.width) && cells[pos.y * dungeonObject.width + pos.x] == EMPTY) {
                cells[pos.y * dungeonObject.width + pos.x] = HALL;
            }
        }
        int hhl = hhld(gen) + dungeonObject.probabilitySet.hhls;
        end = pos.x + hhl;
        for (; pos.x != end; ++pos.x) {
            if (onField(pos, dungeonObject.height, dungeonObject.width) && cells[pos.y * dungeonObject.width + pos.x] == EMPTY) {
                cells[pos.y * dungeonObject.width + pos.x] = HALL;
            }
        }
        hhl = hhld(gen) + dungeonObject.probabilitySet.hhls;
        end = pos.x - hhl;
        for (; pos.x != end; --pos.x) {
            if (onField(pos, dungeonObject.height, dungeonObject.width) && cells[pos.y * dungeonObject.width + pos.x] == EMPTY) {
                cells[pos.y * dungeonObject.width + pos.x] = HALL;
            }
        }
    }

    for (int y{0}; y < dungeonObject.height; ++y) {
        for (int x{0}; x < dungeonObject.width; ++x) {
            if (cells[y * dungeonObject.width + x] == HALL) {
                int count{0};
                Position current{y, x};
                Position next;
                do {
                    if (cells[current.y * dungeonObject.width + current.x] != HALL) break;
                    count = 0;
                    for (auto d:adjacent) {
                        auto np = d + current;
                        if (onField(np, dungeonObject.height, dungeonObject.width) && (cells[np.y * dungeonObject.width + np.x] == HALL || cells[np.y * dungeonObject.width + np.x] == FLOOR)) {
                            ++count;
                            next = np;
                        }
                    }
                    if (count <= 1) {
                        cells[current.y * dungeonObject.width + current.x] = EMPTY;
                        current = next;
                    }
                } while (count == 1);
            }
        }
    }
    for (int y{0}; y < dungeonObject.height; ++y) {
        for (int x{0}; x < dungeonObject.width; ++x) {
            if (cells[y * dungeonObject.width + x] == HALL) {
                for (auto d:adjacent) {
                    auto p = d + Position{y, x};
                    if (onField(p, dungeonObject.height, dungeonObject.width) && cells[p.y * dungeonObject.width + p.x] == FLOOR) {
                        cells[y * dungeonObject.width + x] = OPEN_DOOR;
                    }
                }
            }
        }
    }

    for (int y{0}; y < dungeonObject.height; ++y) {
        for (int x{0}; x < dungeonObject.width; ++x) {
            if (cells[y * dungeonObject.width + x] == OPEN_DOOR) {
                bool noHall{true};
                for (auto d:adjacent) {
                    auto p = d + Position{y, x};
                    if (onField(p, dungeonObject.height, dungeonObject.width) && cells[p.y * dungeonObject.width + p.x] == HALL) {
                        noHall = false;
                        break;
                    }
                }
                if (noHall) {
                    cells[y * dungeonObject.width + x] = EMPTY;
                }
            }
        }
    }

    /* !gen halls */

    /* gen walls */
    for (int y{0}; y < dungeonObject.height; ++y) {
        for (int x{0}; x < dungeonObject.width; ++x) {
            if (cells[y*dungeonObject.width+x] == EMPTY){
                for(auto d:around) {
                    auto p = d+Position{y,x};
                    if (onField(p, dungeonObject.height, dungeonObject.width) && cells[p.y*dungeonObject.width+p.x] == FLOOR) {
                        cells[y*dungeonObject.width+x] = WALL;
                        break;
                    }
                }
            }
        }
    }
    /* !gen walls */

    auto dungeon = make_shared<Dungeon>(state, idMapping[dungeonObject.id], dungeonObject.width, dungeonObject.height);

    for (int y{0}; y < dungeonObject.height; ++y) {
        for (int x{0}; x < dungeonObject.width; ++x) {
            dungeon->initializeCell({y, x}, cells[y * dungeonObject.width + x]);
        }
    }

    int stairRoom = uniform_int_distribution<>(0, roomContents.size() - 1)(gen);
    int spawnRoom;
    if (roomContents.size() == 1) {
        spawnRoom = stairRoom;
    }else{
        spawnRoom = uniform_int_distribution<>(0, roomContents.size() - 2)(gen);
        if (spawnRoom == stairRoom) ++spawnRoom;
    }


    auto stair = make_shared<Entity>();
    stair->representation = '\\';
    stair->move(roomContents[stairRoom][uniform_int_distribution<>(0, roomContents[stairRoom].size() - 1)(gen)]);
    auto stairHandler = make_shared<Stair>(dungeonObject.next != -1 ? idMapping[dungeonObject.next] : Game::nextId(), dungeon, state->loader->getId("player"));
    stair->addListener(stairHandler);
    dungeon->initializeEntity(stair);
    stair->create();

    dungeon->spawnPoint = roomContents[spawnRoom][uniform_int_distribution<>(0, roomContents[spawnRoom].size() - 1)(gen)];

    int remainingCells{-2};
    for (auto &room:roomContents) remainingCells += room.size();
    for (auto spawnSet:dungeonObject.probabilitySet.spawnSets) {
        for(int i {0}; i < spawnSet.amount; ++i) {
            int selection{uniform_int_distribution<>(0, spawnSet.getTotalWeight() - 1)(gen)};
            //cerr << selection << endl;
            auto set = spawnSet.getGroup(selection);

            int posIndex = uniform_int_distribution<>(0, remainingCells - 1)(gen);
            bool posFound{false};
            for (auto &room:roomContents) {
                while (posIndex < room.size()) {
                    if (dungeon->getEntityAt(room[posIndex])) {
                        ++posIndex;
                    } else {
                        posFound = true;
                        for (auto &entity:set) {
                            shared_ptr<Entity> entityObj;
                            switch (state->library.getType(entity)) {
                                case Library::MOB:
                                    entityObj = state->library.getAMob(entity);
                                    break;
                                case Library::ITEM:
                                    entityObj = state->library.getAnItem(entity);
                                    break;
                                case Library::CONSUMABLE:
                                    entityObj = state->library.getAConsumable(entity);
                                    break;
                                case Library::EQUIPPABLE:
                                    entityObj = state->library.getAnEquippable(entity);
                                    break;
                                default:
                                    cerr << "NO SUCH ENTITY: " << entity << endl;
                                    break;
                            }
                            entityObj->move(room[posIndex]);
                            dungeon->initializeEntity(entityObj);
                            entityObj->create();
                        }
                        break;
                    }
                }
                posIndex -= room.size();
                if (posFound) break;
            }
        }
    }

    return dungeon;
}

RandomLevelLoader::ProbabilitySet::ProbabilitySet() {}

RandomLevelLoader::DungeonObject::DungeonObject() {}
