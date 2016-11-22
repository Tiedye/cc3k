#include "Loader.h"
#include "../Race.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../entity/Character.h"

using namespace std;

Loader::Loader() : typeTable{{"player", 1}, {"mob", 2}} {

}

Loader::Loader(std::string gameFile) : Loader() {
    parseFile(gameFile);
}

int Loader::getId(std::string name) const {
    return typeTable.at(name);
}

string getPath(string file) {
    size_t found {file.find_last_of("/\\")};
    return file.substr(0, found);
}

void Loader::parseFile(string gameFile) {
    parseSets(gameFile);
    parseRest(gameFile);
};

void Loader::parseSets(std::string gameFile) {
    ifstream s;
    s.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        s.open(gameFile);

        while (!s.eof()) {
            string command;
            s >> command;
            if (command == "include") {
                string file;
                s >> file;
                parseSets(getPath(gameFile) + "/" + file + ".tdat");
            } else if (command == "start") {
                string object;
                s >> object;
                if (object == "set") {
                    parseSet(s);
                } else {
                    string buf;
                    do {
                        s >> buf;
                    } while (buf != "end");
                }
            }
        }
    } catch (const ifstream::failure &e) {
        // TODO, error on no file
    }
    s.close();
}

void Loader::parseRest(std::string gameFile) {    ifstream s;
    s.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        s.open(gameFile);

        while (!s.eof()) {
            string command;
            s >> command;
            if (command == "include") {
                string file;
                s >> file;
                parseRest(getPath(gameFile) + "/" + file + ".tdat");
            } else if (command == "start") {
                string object;
                s >> object;
                if (object == "race"){
                    parseRace(s);
                } else if (object == "item"){
                    parseItem(s);
                } else if (object == "mob"){
                    parseMob(s);
                } else if (object == "consumable"){
                    parseConsumable(s);
                } else if (object == "equippable"){
                    parseEquippable(s);
                } else {
                    if (object != "set") {
                        cerr << "Unrecognized object type \"" << object << "\"" << endl;
                    }
                    string buf;
                    do {
                        s >> buf;
                    } while (buf != "end");
                }
            } else {
                cerr << "Unrecognized command \"" << command << "\"" << endl;
            }
        }
    } catch (const ifstream::failure &e) {
        // TODO, error on no file
    }
    s.close();
}

void Loader::parseSet(istream &s) {
    string name;
    s >> name;
    if (setTable.count(name)) {
        cerr << "Duplicate definition of set \"" << name << "\"" << endl;
        return;
    }
    // initialise new unique_ptr in place, then retrieve a reference to the FeatureSet
    FeatureSet &newSet = *setTable.emplace(name, make_unique<FeatureSet>()).first->second;
    string command;
    while(s >> command, command != "end") {
        if (command == "inherit") {
            string inherited;
            s >> inherited;
            if (!setTable.count(inherited)){
                cerr << "Inherited set \"" << inherited << "\" has not been declared yet." << endl;
                continue;
            }
            FeatureSet &inheritedSet = *setTable.at(inherited);
            for(auto listener:inheritedSet.listeners) {
                newSet.listeners.push_back(listener);
            }
            for(StatModifier statModifier:inheritedSet.statModifiers) {
                newSet.statModifiers.push_back(statModifier);
            }
            for(auto action:inheritedSet.actions) {
                newSet.actions.push_back(action);
            }
        } else if (command == "stat") {
            string type, stat;
            s >> type >> stat;

            StatType statType {NONE};
            if (stat == "hp") {
                statType = MAX_HEALTH;
            } else if (stat == "size") {
                statType = SIZE;
            } else if (stat == "init") {
                statType = INITIATIVE;
            } else if (stat == "def") {
                statType = DEFENSE_STRENGTH;
            } else if (stat == "kbr") {
                statType = KNOCKBACK_RESIST;
            } else if (stat == "dodge") {
                statType = DODGE;
            } else if (stat == "atk") {
                statType = ATTACK_STRENGTH;
            } else if (stat == "spell") {
                statType = SPELL_STRENGTH;
            } else if (stat == "speed") {
                statType = SPEED;
            } else if (stat == "ccr") {
                statType = TENACITY;
            } else if (stat == "acc") {
                statType = ACCURACY;
            } else {
                cerr << "Unknown Stat \"" << stat << "\"" << endl;
            }

            if (type == "base") {
                short int value, priority;
                s >> value >> priority;
                newSet.statModifiers.emplace_back(statType, StatModifier::BASE, value, priority);
            } else if (type == "add") {
                int value;
                s >> value;
                newSet.statModifiers.emplace_back(statType, StatModifier::ADD, value);
            } else if (type == "sub") {
                int value;
                s >> value;
                newSet.statModifiers.emplace_back(statType, StatModifier::SUBTRACT, value);
            } else if (type == "mult") {
                int value;
                s >> value;
                newSet.statModifiers.emplace_back(statType, StatModifier::MULTIPLY, value);
            } else if (type == "div") {
                int value;
                s >> value;
                newSet.statModifiers.emplace_back(statType, StatModifier::DIVIDE, value);
            } else if (type == "mdiv") {
                short int num, den;
                s >> num >> den;
                newSet.statModifiers.emplace_back(statType, StatModifier::MULTDIV, num, den);
            }
        } else if (command == "effect") {
            newSet.listeners.emplace_back(loadEffect(s));
        } else if (command == "action") {
            newSet.actions.emplace_back(loadAction(s));
        } else {
            cerr << "Unknown Command \"" << command << "\"" << endl;
        }
    }
}

Listener *Loader::loadEffect(istream &s) {
    string effect;
    s >> effect;
    string drain;
    if (effect == "startHp") {
        s >> drain;
    } else if (effect == "lifeDrain") {
        s >> drain;
    } else if (effect == "allergy") {
        string drain;
        while (s >> drain, drain != "done") {}
    } else if (effect == "consumeMod") {
        s >> drain;
    } else if (effect == "resistance") {
        string drain;
        while (s >> drain, drain != "done") {}
    } else if (effect == "regen") {
        s >> drain;
    } else if (effect == "loot") {
        s >> drain;
    } else if (effect == "venerability") {
        string drain;
        while (s >> drain, drain != "done") {}
    } else if (effect == "depleteOnConsume") {
        s >> drain;
    } else if (effect == "restoreOnConsume") {
        s >> drain;
    } else if (effect == "dropItems") {
        string drain;
        while (s >> drain, drain != "done") {}
    } else {
        cerr << "No such effect \"" << effect << "\"" << endl;
    }
    return nullptr;
}

Action *Loader::loadAction(istream &s) {
    string action;
    s >> action;
    string drain;
    if (action == "melee") {
        s >> drain;
        s >> drain;
    } else if (action == "shoot") {
        s >> drain;
        s >> drain;
        s >> drain;
    } else {
        cerr << "No such action \"" << action << "\"" << endl;
    }
    return nullptr;
}

void Loader::parseRace(istream &s){
    Race race;
    s >> race.name;
    race.id = parseId(race.name);
    string command;
    while(s >> command, command != "end") {
        if (command == "set") {
            string set;
            s >> set;
            if (setTable.count(set)) {
                race.featureSet = setTable.at(set).get();
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else if (command == "desc") {
            string word;
            ostringstream desc;
            while (s >> word, word != "end_desc") {
                if (word == "nl") desc << endl;
                else desc << word;
            }
            race.description = desc.str();
        } else if (command == "shortcut") {
            s >> race.shortCut;
        } else {
            cerr << "Unknown Command \"" << command << "\"" << endl;
        }
    }
    races.emplace(race.id, race);
}

void Loader::parseMob(istream &s) {
    Character *newMob {new Character()};
    string name;
    s >> name;
    int id {parseId(name)};
    mobs.emplace(id, unique_ptr<Character>(newMob));
    newMob->types.insert(id);
    newMob->types.insert(parseId("mob"));
    string command;
    while(s >> command, command != "end") {
        if (command == "set") {
            string set;
            s >> set;
            if (setTable.count(set)) {
                newMob->addFeatureSet(*setTable.at(set));
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else if (command == "controller") {
            newMob->controller = loadController(s);
        } else {
            cerr << "Unknown Command \"" << command << "\"" << endl;
        }
    }
}

Controller *Loader::loadController(istream &s) {
    Controller * newController {nullptr};
    string name;
    while(s >> name, name != "done") {
        if (name == "wander") {

        } else if (name == "localAttack") {

        } else if (name == "merchant") {

        } else if (name == "dragon") {

        } else {
            cerr << "No such controller \"" << name << "\"" << endl;
        }
    }
    controllers.emplace_back(newController);
    return newController;
}

void Loader::parseItem(istream &s) {
    string command;
    while(s >> command, command != "end") {

    }
}

void Loader::parseConsumable(std::istream &s) {
    string command;
    while(s >> command, command != "end") {

    }
}

void Loader::parseEquippable(std::istream &s) {
    string command;
    while(s >> command, command != "end") {

    }
}

int Loader::parseId(std::string name) {
    auto it = typeTable.find(name);
    if (it == typeTable.end()) {
        typeTable.emplace(name, typeTable.size());
        return (int) (typeTable.size() - 1);
    }
    return it->second;
}

const std::map<int, Race> Loader::getClassOptions() const {
    return races;
}

void Loader::loadFile(std::string file) {
    parseFile(file);
}
