#pragma once

#include <string>
#include <map>
#include <iostream>
#include <memory>
class Listener;
class Controller;
class Action;
#include "../FeatureSet.h"
#include "../Race.h"

#include "../entity/Entity.h"

class Loader {
public:
    Loader();
    Loader(std::string gameFile);
    int getId(std::string name) const;
    const std::map<int, Race> getClassOptions() const; // TODO, return more data (in a struct?)
    //Character * getPlayer(int raceSelection);
private:
    void parseFile(std::string gameFile);
    void parseSets(std::string gameFile);
    void parseRest(std::string gameFile);
    void parseSet(std::istream &s);
    void parseRace(std::istream &s);
    void parseMob(std::istream &s);
    void parseItem(std::istream &s);
    Listener * loadEffect(std::istream &s);
    Action * loadAction(std::istream &s);
    Controller * loadController(std::istream &s);
    int parseId(std::string name);

    std::map<std::string, std::unique_ptr<FeatureSet>> setTable;
    std::map<std::string, int> typeTable;

    std::map<int, Race> races;

    std::map<int, std::unique_ptr<Character>> mobs;
};



