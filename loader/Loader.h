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
    void loadFile(std::string file);
    int getId(std::string name) const;

private:
    void parseFile(std::string gameFile);
    void parseSets(std::string gameFile);
    void parseRest(std::string gameFile);
    void parseSet(std::istream &s);
    void parseRace(std::istream &s);
    void parseMob(std::istream &s);
    void parseItem(std::istream &s);
    void parseConsumable(std::istream &s);
    void parseEquippable(std::istream &s);
    std::shared_ptr<Listener> loadEffect(std::istream &s);
    std::shared_ptr<Action> loadAction(std::istream &s);
    std::shared_ptr<Controller> loadController(std::istream &s);
    int parseId(std::string name);

    std::map<std::string, std::shared_ptr<FeatureSet>> setTable;
    std::map<std::string, int> typeTable;

    std::vector<std::shared_ptr<Controller>> controllers; // TODO, necessary?

    const std::weak_ptr<State> state;
};



