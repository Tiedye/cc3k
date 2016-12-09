#pragma once

#include <string>
#include <map>
#include <iostream>
#include <memory>
#include <vector>
#include "../entity/Entity.h"

class Listener;
class Action;
class State;
class Controller;
class FeatureSet;

class Loader {
public:
    Loader(const std::shared_ptr<State> &state);
    void load_file(std::string file);
    int get_id(std::string name) const;
    int parse_id(std::string name);
    std::string get_name(int id) const;

private:
    int next_id = 10000;

    void parse_file(std::string game_file);
    void parse_sets(std::string game_file);
    void parse_rest(std::string game_file);
    void parse_set(std::istream &s);
    void parse_race(std::istream &s);
    void parse_mob(std::istream &s);
    void parse_item(std::istream &s);
    void parse_consumable(std::istream &s);
    void parse_equippable(std::istream &s);

    void read_entity_command(const std::shared_ptr<Entity> &entity, const std::string &command, std::istream &s);
    void read_mob_command(const std::shared_ptr<Character> &entity, const std::string &command, std::istream &s);
    void read_item_command(const std::shared_ptr<Item> &entity, const std::string &command, std::istream &s);
    void read_consumable_command(const std::shared_ptr<Consumable> &entity, const std::string &command, std::istream &s);
    void read_equippable_command(const std::shared_ptr<Equippable> &entity, const std::string &command, std::istream &s);

    std::vector<int> read_list(std::istream &s);
    std::string read_string(std::istream &s);
    std::string read_multiline_string(std::istream &s);

    std::shared_ptr<Listener> load_effect(std::istream &s);
    std::shared_ptr<Action> load_action(std::istream &s);
    std::shared_ptr<Controller> load_controller(std::istream &s);

    std::map<std::string, std::shared_ptr<FeatureSet>> set_table;
    std::map<std::string, int> type_table;
    std::map<int, std::string> type_back_table;
    std::set<int> sets_is_type;

    std::vector<std::shared_ptr<Controller>> controllers; // TODO, necessary?

    const std::weak_ptr<State> state;
};



