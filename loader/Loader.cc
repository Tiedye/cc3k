#include "Loader.h"
#include "../Race.h"

#include <fstream>
#include <sstream>

#include "../entity/Character.h"
#include "../entity/Consumable.h"
#include "../entity/Equippable.h"

#include "../FeatureSet.h"
#include "../State.h"

#include "../effects/effects.h"

#include "../action/MeleeAttack.h"
#include "../action/RangedAttack.h"
#include "../action/EatAction.h"
#include "../action/EquipAction.h"
#include "../action/InteractAction.h"
#include "../action/UnequipAction.h"
#include "../action/WalkAction.h"
#include "../action/WalkAsActionAction.h"

using namespace std;

Loader::Loader(const shared_ptr<State> &state)  : typeTable{{"player", 1}, {"mob", 2}}, state{state} {}

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
            } else if (command == "run") {
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
            } else if (command == "run") {
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

shared_ptr<Listener> Loader::loadEffect(istream &s) {
    string effect;
    s >> effect;
    if (effect == "startHp") {
        int hp;
        s >> hp;
        return make_shared<StartHp>(hp);
    } else if (effect == "lifeDrain") {
        int hp;
        s >> hp;
        return make_shared<LifeDrain>(hp);
    } else if (effect == "allergy") {
        int amount;
        s >> amount;
        string type;
        vector<int> to;
        while (s >> type, type != "done") {
            to.push_back(parseId(type));
        }
        return make_shared<Allergy>(amount, to);
    } else if (effect == "healthGainEnhancer") {
        int numerator, denominator;
        s >> numerator >> denominator;
        string from;
        s >> from;
        return make_shared<HealthGainEnhancer>(numerator, denominator, parseId(from));
    } else if (effect == "damageTakenEnhancer") {
        int numerator, denominator;
        s >> numerator >> denominator;
        string from;
        s >> from;
        return make_shared<DamageTakenEnhancer>(numerator, denominator, parseId(from));
    } else if (effect == "resistance") {
        int numerator, denominator;
        s >> numerator >> denominator;
        string type;
        vector<int> to;
        while (s >> type, type != "done") {
            to.push_back(parseId(type));
        }
        return make_shared<VenerabilityModifier>(denominator, numerator, to);
    } else if (effect == "regen") {
        int amount;
        s >> amount;
        return make_shared<Regen>(amount);
    } else if (effect == "loot") {
        int amount;
        s >> amount;
        return make_shared<Loot>(amount);
    } else if (effect == "venerability") {
        int numerator, denominator;
        s >> numerator >> denominator;
        string type;
        vector<int> to;
        while (s >> type, type != "done") {
            to.push_back(parseId(type));
        }
        return make_shared<VenerabilityModifier>(numerator, denominator, to);
    } else if (effect == "depleteOnConsume") {
        int amount;
        s >> amount;
        return make_shared<DamageOnConsume>(amount);
    } else if (effect == "restoreOnConsume") {
        int amount;
        s >> amount;
        return make_shared<HealOnConsume>(amount);
    } else if (effect == "dropItems") {
        auto dropItems = make_shared<DropItems>(state);
        string token;
        vector<int> itemSet;
        int weight {0};
        while (s >> token, token != "done") {
            if (token == "[") {
                s >> weight;
            } else if (token == "]") {
                dropItems->addItem(weight, itemSet);
                itemSet.clear();
            }
        }
        return dropItems;
    } else {
        cerr << "No such effect \"" << effect << "\"" << endl;
        return nullptr;
    }
}

shared_ptr<Action> Loader::loadAction(istream &s) {
    string action;
    s >> action;
    if (action == "melee") {
        int atkModifier, spellModifier;
        s >> atkModifier >> spellModifier;
        return make_shared<MeleeAttack>(atkModifier, 1, spellModifier, 1);
    } else if (action == "shoot") {
        int atkModifier, spellModifier, range;
        s >> atkModifier >> spellModifier >> range;
        return make_shared<RangedAttack>(atkModifier, 1, spellModifier, 1, range);
    } else if (action == "walk") {
        return make_shared<WalkAction>();
    } else if (action == "walkAsAction") {
        return make_shared<WalkAsActionAction>();
    } else if (action == "eat") {
        return make_shared<EatAction>();
    } else if (action == "interact") {
        return make_shared<InteractAction>();
    } else if (action == "equip") {
        return make_shared<EquipAction>();
    } else if (action == "unequip") {
        return make_shared<UnequipAction>();
    } else {
        cerr << "No such action \"" << action << "\"" << endl;
        return nullptr;
    }
}

void Loader::parseRace(istream &s){
    auto race = make_unique<Race>();
    s >> race->name;
    race->id = parseId(race->name);
    string command;
    while(s >> command, command != "end") {
        if (command == "set") {
            string set;
            s >> set;
            if (setTable.count(set)) {
                race->featureSet = setTable[set];
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else if (command == "slots") {
            string slot;
            while(s >> slot, slot != "done") {
                race->slots.insert(parseId(slot));
            }
        } else if (command == "desc") {
            string word;
            ostringstream desc;
            while (s >> word, word != "end_desc") {
                if (word == "nl") desc << endl;
                else desc << word;
            }
            race->description = desc.str();
        } else if (command == "shortcut") {
            s >> race->shortcut;
        } else {
            cerr << "Unknown Command \"" << command << "\"" << endl;
        }
    }
    state.lock()->library.addRace(race->id, move(race));
}

void Loader::parseMob(istream &s) {
    unique_ptr<Character> newMob {make_unique<Character>()};
    string name;
    s >> name;
    int id {parseId(name)};
    newMob->types.insert(id);
    newMob->types.insert(parseId("mob"));
    string command;
    while(s >> command, command != "end") {
        if (command == "set") {
            string set;
            s >> set;
            if (setTable.count(set)) {
                newMob->addFeatureSet(*setTable[set]);
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else if (command == "controller") {
            newMob->controller = loadController(s);
        } else {
            cerr << "Unknown Command \"" << command << "\"" << endl;
        }
    }
    state.lock()->library.addMob(id, move(newMob));
}

shared_ptr<Controller> Loader::loadController(istream &s) {
    shared_ptr<Controller> newController;
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
    controllers.push_back(newController);  // TODO, necedssary to track controllers in loader?
    return newController;
}

void Loader::parseItem(istream &s) {
    auto newItem = make_unique<Item>();
    string name;
    s >> name;
    int id {parseId(name)};
    newItem->types.insert(id);
    newItem->types.insert(parseId("item"));
    string command;
    while(s >> command, command != "end") {
        if (command == "set") {
            string set;
            s >> set;
            if (setTable.count(set)) {
                newItem->addFeatureSet(*setTable[set]);
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else if (command == "value") {
            int value;
            s >> value;
            newItem->value = value;
        } else {
            cerr << "Unknown Command \"" << command << "\"" << endl;
        }
    }
    state.lock()->library.addItem(id, move(newItem));
}

void Loader::parseConsumable(std::istream &s) {
    auto newConsumable = make_unique<Consumable>();
    string name;
    s >> name;
    int id {parseId(name)};
    newConsumable->types.insert(id);
    newConsumable->types.insert(parseId("item"));
    newConsumable->types.insert(parseId("consumable"));
    string command;
    while(s >> command, command != "end") {
        if (command == "set") {
            string set;
            s >> set;
            if (setTable.count(set)) {
                newConsumable->addFeatureSet(*setTable[set]);
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else if (command == "value") {
            int value;
            s >> value;
            newConsumable->value = value;
        } else if (command == "consumed") {
            int numTurns; // TODO check syntax error
            s >> numTurns;
            string set;
            s >> set;
            if (setTable.count(set)) {
                newConsumable->set = setTable[set];
                newConsumable->numTurns = numTurns;
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else if (command == "effectType"){
            string effectType;
            s >> effectType;
            if (effectType == "positive") {
                newConsumable->effectType = POSITIVE;
            } else if (effectType == "negative") {
                newConsumable->effectType = NEGATIVE;
            } else if (effectType == "neutral") {
                newConsumable->effectType = NEUTRAL;
            } else {
                cerr << "Unknown effectType \"" << effectType << "\"" << endl;
            }
        } else {
            cerr << "Unknown Command \"" << command << "\"" << endl;
        }
    }
    state.lock()->library.addConsumable(id, move(newConsumable));
}

void Loader::parseEquippable(std::istream &s) {
    auto newEquippable = make_unique<Equippable>();
    string name;
    s >> name;
    int id {parseId(name)};
    newEquippable->types.insert(id);
    newEquippable->types.insert(parseId("item"));
    newEquippable->types.insert(parseId("equippable"));
    string command;
    while(s >> command, command != "end") {
        if (command == "set") {
            string set;
            s >> set;
            if (setTable.count(set)) {
                newEquippable->addFeatureSet(*setTable[set]);
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else if (command == "value") {
            int value;
            s >> value;
            newEquippable->value = value;
        } else if (command == "equipped") {
            string slot;
            s >> slot;
            string set;
            s >> set;
            if (setTable.count(set)) {
                newEquippable->slot = parseId(slot);
                newEquippable->set = setTable[set];
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else {
            cerr << "Unknown Command \"" << command << "\"" << endl;
        }
    }
    state.lock()->library.addEquippable(id, move(newEquippable));
}

int Loader::parseId(std::string name) {
    auto it = typeTable.find(name);
    if (it == typeTable.end()) {
        typeTable.emplace(name, typeTable.size());
        return (int) (typeTable.size() - 1);
    }
    return it->second;
}

void Loader::loadFile(std::string file) {
    parseFile(file);
}
