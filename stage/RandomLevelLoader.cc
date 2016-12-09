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

RandomLevelLoader::RandomLevelLoader(const std::shared_ptr<State> &state, const std::string &config_file) : Stage(state), config_file(config_file) {}

void RandomLevelLoader::SpawnSet::add_group(int weight, const std::vector<int> &items) {
    total_weight += weight;
    groups.emplace(weight, items);
}

std::vector<int> RandomLevelLoader::SpawnSet::get_group(int selection) {
    for (auto &group:groups) {
        if (selection < group.first) {
            return group.second;
        }
        selection -= group.first;
    }
    return groups.begin()->second;
}

int RandomLevelLoader::SpawnSet::get_total_weight() const {
    return total_weight;
}

RandomLevelLoader::SpawnSet::SpawnSet() {}

int RandomLevelLoader::run(Game &game) {
    ifstream s{config_file};
    if (s) {
        int initial_dungeon {-1};

        string command;
        while (s >> command) {
            if (command == "start") {
                string object;
                s >> object;
                if (object == "probability_set") {
                    parse_probability_set(s);
                } else if (object == "dungeon") {
                    parse_dungeon(s);
                } else {
                    cerr << "Unrecognized command \"" << command << "\"" << endl;
                }
            } else if (command == "initial") {
                string dungeon;
                s >> dungeon;
                initial_dungeon = state->loader->parse_id(dungeon);
            }
        }

        // make dungeons

        for (auto &d:dungeon_objects) {
            id_mapping.emplace(d.second.id, Game::get_id());
        }

        for (auto &d:dungeon_objects) {
            auto dungeon = gen_dungeon(d.second);
            game.add_stage(1, dungeon);
        }
        game.add_stage(1, make_shared<EndScreen>(state, 0));
        return id_mapping[initial_dungeon];

    } else {
        cerr << "Could not load level file \"" << config_file << "\"" << endl;
        return -1;
    }
}

void RandomLevelLoader::parse_probability_set(std::istream &s) {
    ProbabilitySet probability_set;
    string name;
    s >> name;
    int id{state->loader->parse_id(name)};
    string command;
    while (s >> command, command != "end") {
        if (command == "param") {
            string param;
            int value;
            s >> param >> value;
            if (param == "cell_height_mean") { probability_set.chm = value; }
            else if (param == "cell_height_shift") { probability_set.chs = value; }
            else if (param == "cell_width_mean") { probability_set.cwm = value; }
            else if (param == "cell_width_shift") { probability_set.cws = value; }
            else if (param == "cell_count_mean") { probability_set.ccm = value; }
            else if (param == "cell_count_shift") { probability_set.ccs = value; }
            else if (param == "vertical_hall_length_mean") { probability_set.vhlm = value; }
            else if (param == "vertical_hall_length_shift") { probability_set.vhls = value; }
            else if (param == "horizontal_hall_length_mean") { probability_set.hhlm = value; }
            else if (param == "horizontal_hall_length_shift") { probability_set.hhls = value; }
        } else if (command == "spawn_set") {
            probability_set.spawn_sets.push_back(parse_spawn_set(s));
        } else {
            cerr << "Unknown Command \"" << command << "\" when parsing probability set" << endl;
        }
    }
    probability_sets.emplace(id, move(probability_set));
}

void RandomLevelLoader::parse_dungeon(std::istream &s) {
    DungeonObject dungeon_object;
    string dungeon_name;
    s >> dungeon_name;
    int id{state->loader->parse_id(dungeon_name)};
    dungeon_object.id = id;
    string command;
    while (s >> command, command != "end") {
        if (command == "set") {
            string name;
            s >> name;
            dungeon_object.probability_set = probability_sets.at(state->loader->parse_id(name));
        } else if (command == "next") {
            string name;
            s >> name;
            dungeon_object.next = state->loader->parse_id(name);
        } else if (command == "size") {
            int height, width;
            s >> height >> width;
            dungeon_object.height = height;
            dungeon_object.width = width;
        } else {
            cerr << "Unknown Command \"" << command << "\" when parsing dungeon object" << endl;
        }
    }
    dungeon_objects.emplace(id, move(dungeon_object));
}

RandomLevelLoader::SpawnSet RandomLevelLoader::parse_spawn_set(std::istream &s) {
    SpawnSet spawn_set;
    int amount;
    s >> amount;
    spawn_set.amount = amount;
    string token;
    int weight {0};
    vector<int> current;
    while (s >> token, token != "done") {
        if (token == "[") {
            s >> weight;
        } else if (token == "]") {
            spawn_set.add_group(weight, current);
            current.clear();
            weight = -1;
        } else {
            if (weight == -1) {
                cerr << "Must start spawn group with \"[\" not " << token << endl;
            } else {
                current.push_back(state->loader->parse_id(token));
            }
        }
    }
    return spawn_set;
}

bool on_field(const Position position, const int height, const int width) {
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

std::shared_ptr<Dungeon> RandomLevelLoader::gen_dungeon(const DungeonObject &dungeon_object) {
    vector<CellType> cells(dungeon_object.width * dungeon_object.height, EMPTY);
    vector<int> rooms(dungeon_object.width * dungeon_object.height, -1);

    poisson_distribution<> h{dungeon_object.probability_set.chm};
    poisson_distribution<> w{dungeon_object.probability_set.cwm};

    poisson_distribution<> c{dungeon_object.probability_set.ccm};

    uniform_int_distribution<> all;
    auto &gen = state->gen;

    /* gen rooms */

    int cc = c(gen) + dungeon_object.probability_set.ccs;
    for (int cn{0}; cn < cc; ++cn) {
        int ch = h(gen) + dungeon_object.probability_set.chs;
        int cw = w(gen) + dungeon_object.probability_set.cws;
        int y = all(gen) % (dungeon_object.height - 2 - ch) + 1;
        int x = all(gen) % (dungeon_object.width - 2 - cw) + 1;

        for (int yv{y}; yv < y + ch; ++yv) {
            for (int xv{x}; xv < x + cw; ++xv) {
                cells[yv * dungeon_object.width + xv] = FLOOR;
            }
        }
    }

    /* !gen rooms */
    /* gen halls */

    vector<vector<Position>> room_contents;

    for (int y{0}; y < dungeon_object.height; ++y) {
        for (int x{0}; x < dungeon_object.width; ++x) {
            rooms[y * dungeon_object.width + x] = -1;
        }
    }

    int room_ids{0};
    for (int y{0}; y < dungeon_object.height; ++y) {
        for (int x{0}; x < dungeon_object.width; ++x) {
            if (cells[y * dungeon_object.width + x] == FLOOR && rooms[y * dungeon_object.width + x] == -1) {
                int room_id = room_ids++;
                vector<Position> contents;
                queue<Position> to_fill;
                to_fill.push({y, x});
                rooms[y * dungeon_object.width + x] = room_id;
                while (!to_fill.empty()) {
                    Position current_position = to_fill.front();
                    contents.push_back(current_position);
                    to_fill.pop();
                    for (Position &delta:around) {
                        auto pos = delta + current_position;
                        if (on_field(pos, dungeon_object.height, dungeon_object.width) && rooms[pos.y * dungeon_object.width + pos.x] == -1 &&
                            cells[pos.y * dungeon_object.width + pos.x] == FLOOR) {
                            rooms[pos.y * dungeon_object.width + pos.x] = room_id;
                            to_fill.push(pos);
                        }
                    }
                }
                room_contents.push_back(move(contents));
                break;
            }
        }
    }

    poisson_distribution<> vhld{dungeon_object.probability_set.vhlm};
    poisson_distribution<> hhld{dungeon_object.probability_set.hhlm};

    for (auto &room:room_contents) {
        auto pos = room[uniform_int_distribution<>(0, room.size() - 1)(gen)];
        int vhl = vhld(gen) + dungeon_object.probability_set.vhls;
        int end = pos.y + vhl;
        for (; pos.y != end; ++pos.y) {
            if (on_field(pos, dungeon_object.height, dungeon_object.width) && cells[pos.y * dungeon_object.width + pos.x] == EMPTY) {
                cells[pos.y * dungeon_object.width + pos.x] = HALL;
            }
        }
        vhl = vhld(gen) + dungeon_object.probability_set.vhls;
        end = pos.y - vhl;
        for (; pos.y != end; --pos.y) {
            if (on_field(pos, dungeon_object.height, dungeon_object.width) && cells[pos.y * dungeon_object.width + pos.x] == EMPTY) {
                cells[pos.y * dungeon_object.width + pos.x] = HALL;
            }
        }
        int hhl = hhld(gen) + dungeon_object.probability_set.hhls;
        end = pos.x + hhl;
        for (; pos.x != end; ++pos.x) {
            if (on_field(pos, dungeon_object.height, dungeon_object.width) && cells[pos.y * dungeon_object.width + pos.x] == EMPTY) {
                cells[pos.y * dungeon_object.width + pos.x] = HALL;
            }
        }
        hhl = hhld(gen) + dungeon_object.probability_set.hhls;
        end = pos.x - hhl;
        for (; pos.x != end; --pos.x) {
            if (on_field(pos, dungeon_object.height, dungeon_object.width) && cells[pos.y * dungeon_object.width + pos.x] == EMPTY) {
                cells[pos.y * dungeon_object.width + pos.x] = HALL;
            }
        }
    }

    for (int y{0}; y < dungeon_object.height; ++y) {
        for (int x{0}; x < dungeon_object.width; ++x) {
            if (cells[y * dungeon_object.width + x] == HALL) {
                int count{0};
                Position current{y, x};
                Position next;
                do {
                    if (cells[current.y * dungeon_object.width + current.x] != HALL) break;
                    count = 0;
                    for (auto d:adjacent) {
                        auto np = d + current;
                        if (on_field(np, dungeon_object.height, dungeon_object.width) && (cells[np.y * dungeon_object.width + np.x] == HALL || cells[np.y * dungeon_object.width + np.x] == FLOOR)) {
                            ++count;
                            next = np;
                        }
                    }
                    if (count <= 1) {
                        cells[current.y * dungeon_object.width + current.x] = EMPTY;
                        current = next;
                    }
                } while (count == 1);
            }
        }
    }
    for (int y{0}; y < dungeon_object.height; ++y) {
        for (int x{0}; x < dungeon_object.width; ++x) {
            if (cells[y * dungeon_object.width + x] == HALL) {
                for (auto d:adjacent) {
                    auto p = d + Position{y, x};
                    if (on_field(p, dungeon_object.height, dungeon_object.width) && cells[p.y * dungeon_object.width + p.x] == FLOOR) {
                        cells[y * dungeon_object.width + x] = OPEN_DOOR;
                    }
                }
            }
        }
    }

    for (int y{0}; y < dungeon_object.height; ++y) {
        for (int x{0}; x < dungeon_object.width; ++x) {
            if (cells[y * dungeon_object.width + x] == OPEN_DOOR) {
                bool no_hall{true};
                for (auto d:adjacent) {
                    auto p = d + Position{y, x};
                    if (on_field(p, dungeon_object.height, dungeon_object.width) && cells[p.y * dungeon_object.width + p.x] == HALL) {
                        no_hall = false;
                        break;
                    }
                }
                if (no_hall) {
                    cells[y * dungeon_object.width + x] = EMPTY;
                }
            }
        }
    }

    /* !gen halls */

    /* gen walls */
    for (int y{0}; y < dungeon_object.height; ++y) {
        for (int x{0}; x < dungeon_object.width; ++x) {
            if (cells[y*dungeon_object.width+x] == EMPTY){
                for(auto d:around) {
                    auto p = d+Position{y,x};
                    if (on_field(p, dungeon_object.height, dungeon_object.width) && cells[p.y*dungeon_object.width+p.x] == FLOOR) {
                        cells[y*dungeon_object.width+x] = WALL;
                        break;
                    }
                }
            }
        }
    }
    /* !gen walls */

    auto dungeon = make_shared<Dungeon>(state, id_mapping[dungeon_object.id], dungeon_object.width, dungeon_object.height);

    for (int y{0}; y < dungeon_object.height; ++y) {
        for (int x{0}; x < dungeon_object.width; ++x) {
            dungeon->initialize_cell({y, x}, cells[y * dungeon_object.width + x]);
        }
    }

    int stair_room = uniform_int_distribution<>(0, room_contents.size() - 1)(gen);
    int spawn_room;
    if (room_contents.size() == 1) {
        spawn_room = stair_room;
    }else{
        spawn_room = uniform_int_distribution<>(0, room_contents.size() - 2)(gen);
        if (spawn_room == stair_room) ++spawn_room;
    }


    auto stair = make_shared<Entity>();
    stair->representation = '\\';
    stair->move(room_contents[stair_room][uniform_int_distribution<>(0, room_contents[stair_room].size() - 1)(gen)]);
    auto stair_handler = make_shared<Stair>(dungeon_object.next != -1 ? id_mapping[dungeon_object.next] : Game::next_id(), dungeon, state->loader->get_id("player"));
    stair->add_listener(stair_handler);
    dungeon->initialize_entity(stair);
    stair->create();

    dungeon->spawn_point = room_contents[spawn_room][uniform_int_distribution<>(0, room_contents[spawn_room].size() - 1)(gen)];

    int remaining_cells{-2};
    for (auto &room:room_contents) remaining_cells += room.size();
    for (auto spawn_set:dungeon_object.probability_set.spawn_sets) {
        for(int i {0}; i < spawn_set.amount; ++i) {
            int selection{uniform_int_distribution<>(0, spawn_set.get_total_weight() - 1)(gen)};
            //cerr << selection << endl;
            auto set = spawn_set.get_group(selection);

            int pos_index = uniform_int_distribution<>(0, remaining_cells - 1)(gen);
            bool pos_found{false};
            for (auto &room:room_contents) {
                while (pos_index < room.size()) {
                    if (dungeon->get_entity_at(room[pos_index])) {
                        ++pos_index;
                    } else {
                        pos_found = true;
                        for (auto &entity:set) {
                            shared_ptr<Entity> entity_obj;
                            switch (state->library.get_type(entity)) {
                                case Library::MOB:
                                    entity_obj = state->library.get_a_mob(entity);
                                    break;
                                case Library::ITEM:
                                    entity_obj = state->library.get_an_item(entity);
                                    break;
                                case Library::CONSUMABLE:
                                    entity_obj = state->library.get_a_consumable(entity);
                                    break;
                                case Library::EQUIPPABLE:
                                    entity_obj = state->library.get_an_equippable(entity);
                                    break;
                                default:
                                    cerr << "NO SUCH ENTITY: " << entity << endl;
                                    break;
                            }
                            entity_obj->move(room[pos_index]);
                            dungeon->initialize_entity(entity_obj);
                            entity_obj->create();
                        }
                        break;
                    }
                }
                pos_index -= room.size();
                if (pos_found) break;
            }
        }
    }

    return dungeon;
}

RandomLevelLoader::ProbabilitySet::ProbabilitySet() {}

RandomLevelLoader::DungeonObject::DungeonObject() {}
