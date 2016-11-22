#pragma once

#include <string>
#include <map>
#include <iostream>
#include <memory>
class Listener;
class Action;
#include "../FeatureSet.h"
#include "../Race.h"

#include "../entity/Entity.h"
#include "../controller/Controller.h"

class Loader {
public:
    Loader();
    Loader(std::string gameFile);
    void loadFile(std::string file);
    int getId(std::string name) const;
    const std::map<int, Race> getClassOptions() const;
    //Character * getPlayer(int raceSelection);
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

    std::map<std::string, std::unique_ptr<FeatureSet>> setTable;
    std::map<std::string, int> typeTable;

    std::map<int, Race> races;

    std::map<int, std::unique_ptr<Character>> mobs;

    std::vector<std::unique_ptr<Controller>> controllers;
};



