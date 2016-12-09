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
#include "../controller/Wander.h"
#include "../controller/LocalAttackPlayer.h"
#include "../effects/StateSetOnAttacked.h"
#include "../controller/StateConditional.h"
#include "../controller/Guard.h"

using namespace std;

Loader::Loader(const shared_ptr<State> &state)  : type_table{{"player", 10}, {"mob", 11}, {"floor", CellType::FLOOR}, {"door", CellType::OPEN_DOOR}, {"hall", CellType::HALL}}, state{state} {}

int Loader::get_id(std::string name) const {
    return type_table.at(name);
}

string Loader::get_name(int id) const {
    return type_back_table.at(id);
}

string get_path(string file) {
    size_t found {file.find_last_of("/\\")};
    return file.substr(0, found);
}

void Loader::parse_file(string game_file) {
    parse_sets(game_file);
    parse_rest(game_file);
};

void Loader::parse_sets(std::string game_file) {
    ifstream s {game_file};
    if (s) {
        string command;
        while (s >> command) {
            if (command == "include") {
                string file;
                s >> file;
                parse_sets(get_path(game_file) + "/" + file + ".tdat");
            } else if (command == "start") {
                string object;
                s >> object;
                if (object == "set") {
                    parse_set(s);
                } else {
                    string buf;
                    do {
                        s >> buf;
                    } while (buf != "end");
                }
            }
        }
    } else {
        // TODO, error on no file
    }
    s.close();
}

void Loader::parse_rest(std::string game_file) {
    ifstream s {game_file};
    if (s) {
        string command;
        while (s >> command) {
            if (command == "include") {
                string file;
                s >> file;
                parse_rest(get_path(game_file) + "/" + file + ".tdat");
            } else if (command == "start") {
                string object;
                s >> object;
                if (object == "race") {
                    parse_race(s);
                } else if (object == "item") {
                    parse_item(s);
                } else if (object == "mob") {
                    parse_mob(s);
                } else if (object == "consumable") {
                    parse_consumable(s);
                } else if (object == "equippable") {
                    parse_equippable(s);
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
    } else {
        // TODO err on cant open file
    }
    s.close();
}

void Loader::parse_set(istream &s) {
    string name;
    s >> name;
    if (set_table.count(name)) {
        cerr << "Duplicate definition of set \"" << name << "\"" << endl;
        return;
    }
    // initialise new unique_ptr in place, then retrieve a reference to the FeatureSet
    FeatureSet &new_set = *set_table.emplace(name, make_unique<FeatureSet>()).first->second;
    string command;
    while(s >> command, command != "end") {
        if (command == "inherit") {
            string inherited;
            s >> inherited;
            if (!set_table.count(inherited)){
                cerr << "Inherited set \"" << inherited << "\" has not been declared yet." << endl;
                continue;
            }
            FeatureSet &inherited_set = *set_table.at(inherited);
            for(auto listener:inherited_set.listeners) {
                new_set.listeners.push_back(listener);
            }
            for(StatModifier stat_modifier:inherited_set.stat_modifiers) {
                new_set.stat_modifiers.push_back(stat_modifier);
            }
            for(auto action:inherited_set.actions) {
                new_set.actions.push_back(action);
            }
        } else if (command == "stat") {
            string type, stat;
            s >> type >> stat;

            StatType stat_type {NONE};
            if (stat == "hp") {
                stat_type = MAX_HEALTH;
            } else if (stat == "size") {
                stat_type = SIZE;
            } else if (stat == "init") {
                stat_type = INITIATIVE;
            } else if (stat == "def") {
                stat_type = DEFENSE_STRENGTH;
            } else if (stat == "kbr") {
                stat_type = KNOCKBACK_RESIST;
            } else if (stat == "dodge") {
                stat_type = DODGE;
            } else if (stat == "atk") {
                stat_type = ATTACK_STRENGTH;
            } else if (stat == "spell") {
                stat_type = SPELL_STRENGTH;
            } else if (stat == "speed") {
                stat_type = SPEED;
            } else if (stat == "ccr") {
                stat_type = TENACITY;
            } else if (stat == "acc") {
                stat_type = ACCURACY;
            } else {
                cerr << "Unknown Stat \"" << stat << "\"" << endl;
            }

            if (type == "base") {
                short int value, priority;
                s >> value >> priority;
                new_set.stat_modifiers.emplace_back(stat_type, StatModifier::BASE, value, priority);
            } else if (type == "add") {
                int value;
                s >> value;
                new_set.stat_modifiers.emplace_back(stat_type, StatModifier::ADD, value);
            } else if (type == "sub") {
                int value;
                s >> value;
                new_set.stat_modifiers.emplace_back(stat_type, StatModifier::SUBTRACT, value);
            } else if (type == "mult") {
                int value;
                s >> value;
                new_set.stat_modifiers.emplace_back(stat_type, StatModifier::MULTIPLY, value);
            } else if (type == "div") {
                int value;
                s >> value;
                new_set.stat_modifiers.emplace_back(stat_type, StatModifier::DIVIDE, value);
            } else if (type == "mdiv") {
                short int num, den;
                s >> num >> den;
                new_set.stat_modifiers.emplace_back(stat_type, StatModifier::MULTDIV, num, den);
            }
        } else if (command == "effect") {
            new_set.listeners.emplace_back(load_effect(s));
        } else if (command == "action") {
            new_set.actions.emplace_back(load_action(s));
        } else if (command == "is_type") {
            sets_is_type.insert(parse_id(name));
        } else {
            cerr << "Unknown Command \"" << command << "\"" << endl;
        }
    }
}

std::vector<int> Loader::read_list(std::istream &s) {
    vector<int> list;
    string item;
    while(s >> item, item != "]" && item != "done") list.push_back(parse_id(item));
    return list;
}

std::string Loader::read_string(std::istream &s) {
    ostringstream new_string;
    string token;
    s >> token;
    new_string << token;
    while(s >> token, token != "done" && token != "]") {
        new_string << " " << token;
    }
    return new_string.str();
}

std::string Loader::read_multiline_string(std::istream &s) {
    ostringstream new_string;
    string token;
    while (s >> token, token != "end_str") {
        if (token == "nl") new_string << endl;
        else new_string << token << " ";
    }
    return new_string.str();
}

shared_ptr<Listener> Loader::load_effect(istream &s) {
    string effect;
    s >> effect;
    if (effect == "start_hp") {
        int hp;
        s >> hp;
        return make_shared<StartHp>(hp);
    } else if (effect == "life_drain") {
        int hp;
        s >> hp;
        return make_shared<LifeDrain>(hp);
    } else if (effect == "allergy") {
        int amount;
        s >> amount;
        string type;
        return make_shared<Allergy>(amount, read_list(s));
    } else if (effect == "health_gain_enhancer") {
        int numerator, denominator;
        s >> numerator >> denominator;
        string from;
        s >> from;
        return make_shared<HealthGainModifier>(numerator, denominator, parse_id(from));
    } else if (effect == "damage_taken_enhancer") {
        int numerator, denominator;
        s >> numerator >> denominator;
        string from;
        s >> from;
        return make_shared<DamageTakenModifier>(numerator, denominator, parse_id(from));
    } else if (effect == "resistance") {
        int numerator, denominator;
        s >> numerator >> denominator;
        string type;
        return make_shared<VenerabilityModifier>(denominator, numerator, read_list(s));
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
        return make_shared<VenerabilityModifier>(numerator, denominator, read_list(s));
    } else if (effect == "deplete_on_consume") {
        int amount;
        s >> amount;
        return make_shared<DamageOnConsume>(amount);
    } else if (effect == "restore_on_consume") {
        int amount;
        s >> amount;
        return make_shared<HealOnConsume>(amount);
    } else if (effect == "drop_items") {
        auto drop_items = make_shared<DropItems>(state);
        string token;
        while (s >> token, token != "done") {
            if (token == "[") {
                int weight;
                s >> weight;
                drop_items->add_item(weight, read_list(s));
            } else {
                cerr << "Not a valid weighted list start \"" << token << "\"" << endl;
            }
        }
        return drop_items;
    } else if (effect == "auto_pickup") {
        return make_shared<TypeAutoPickup>(parse_id("player"));
    } else if (effect == "enhance_temp_effects") {
        short mod_numerator, mod_denominator;
        s >> mod_numerator >> mod_denominator;
        string type;
        return make_shared<EnhanceTemporaryEffects>(mod_numerator, mod_denominator, read_list(s));
    } else if (effect == "set_state_on_attacked") {
        string state_name, by;
        s >> state_name >> by;
        if (!HasAIData::ai_reserved_id(state_name)) HasAIData::ai_reserve_id(state_name);
        return make_shared<StateSetOnAttacked>(HasAIData::ai_get_id(state_name), parse_id(by), state);
    } else if (effect == "add_value_on_hold") {
        return make_shared<AddValueOnHold>();
    } else if (effect == "invulnerable") {
        return make_shared<Invulnerable>();
    } else if (effect == "steal_score") {
        return make_shared<StealScoreOnKill>();
    } else if (effect == "thorns") {
        int numerator, denominator;
        s >> numerator >> denominator;
        return make_shared<Thorns>(numerator, denominator);
    } else {
        cerr << "No such effect \"" << effect << "\"" << endl;
        return nullptr;
    }
}

shared_ptr<Action> Loader::load_action(istream &s) {
    string action;
    s >> action;
    if (action == "melee") {
        int atk_modifier, spell_modifier;
        s >> atk_modifier >> spell_modifier;
        return make_shared<MeleeAttack>(atk_modifier, 1, spell_modifier, 1);
    } else if (action == "shoot") {
        int atk_modifier, spell_modifier, range;
        s >> atk_modifier >> spell_modifier >> range;
        return make_shared<RangedAttack>(atk_modifier, 1, spell_modifier, 1, range);
    } else if (action == "walk") {
        return make_shared<WalkAction>(read_list(s));
    } else if (action == "walk_as_action") {
        return make_shared<WalkAsActionAction>(read_list(s));
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

void Loader::parse_race(istream &s){
    auto race = make_unique<Race>();
    s >> race->name;
    race->id = parse_id(race->name);
    string command;
    while(s >> command, command != "end") {
        if (command == "set") {
            string set;
            s >> set;
            if (set_table.count(set)) {
                race->feature_set = set_table[set];
            } else {
                cerr << "Could not find set \"" << set << "\"" << endl;
            }
        } else if (command == "slots") {
            for(auto slot:read_list(s)) {
                race->slots.insert(slot);
            }
        } else if (command == "desc") {
            race->description = read_multiline_string(s);
        } else if (command == "shortcut") {
            s >> race->shortcut;
        } else {
            cerr << "Unknown Command \"" << command << "\" when parsing Race" << endl;
        }
    }
    int id {race->id};
    state.lock()->library.add_race(id, move(race));
}

shared_ptr<Controller> Loader::load_controller(istream &s) {
    shared_ptr<Controller> new_controller;
    string name;
    while(s >> name, name != "done" && name!= "]") {
        if (name == "wander") {
            new_controller = make_shared<Wander>();
        } else if (name == "local_attack") {
            new_controller = make_shared<LocalAttackPlayer>(new_controller);
        } else if (name == "state_if") {
            string state_name, token;
            s >> state_name >> token;
            if (token != "[") {
                cerr << "Invalid controller start block \"" << token << "\", must be \"[\"" << endl;
            }
            auto true_controller = load_controller(s);
            s >> token;
            if (token != "[") {
                cerr << "Invalid controller start block \"" << token << "\", must be \"[\"" << endl;
            }
            auto false_controller = load_controller(s);
            if (!HasAIData::ai_reserved_id(state_name)) HasAIData::ai_reserve_id(state_name);
            new_controller = make_shared<StateConditional>(HasAIData::ai_get_id(state_name), true_controller, false_controller);
        } else if (name == "guard") {
            string guarded;
            s >> guarded;
            string token;
            s >> token;
            if (token != "[") {
                cerr << "Invalid identifier start block \"" << token << "\", must be \"[\"" << endl;
            }
            new_controller = make_shared<Guard>(parse_id(guarded), read_list(s));
        } else {
            cerr << "No such controller \"" << name << "\"" << endl;
        }
    }
    controllers.push_back(new_controller);  // TODO, necedssary to track controllers in loader?
    return new_controller;
}

void Loader::parse_mob(istream &s) {
    auto new_mob = make_shared<Character>();
    string name;
    s >> name;
    int id {parse_id(name)};
    new_mob->types.insert(id);
    new_mob->types.insert(parse_id("mob"));
    string command;
    while(s >> command, command != "end") {
        read_mob_command(new_mob, command, s);
    }
    state.lock()->library.add_mob(id, new_mob);
}

void Loader::parse_item(istream &s) {
    auto new_item = make_shared<Item>();
    string name;
    s >> name;
    int id {parse_id(name)};
    new_item->types.insert(id);
    new_item->types.insert(parse_id("item"));
    string command;
    while(s >> command, command != "end") {
        read_item_command(new_item, command, s);
    }
    state.lock()->library.add_item(id, new_item);
}

void Loader::parse_consumable(std::istream &s) {
    auto new_consumable = make_shared<Consumable>();
    string name;
    s >> name;
    int id {parse_id(name)};
    new_consumable->types.insert(id);
    new_consumable->types.insert(parse_id("item"));
    new_consumable->types.insert(parse_id("consumable"));
    string command;
    while(s >> command, command != "end") {
        read_consumable_command(new_consumable, command, s);
    }
    state.lock()->library.add_consumable(id, new_consumable);
}

void Loader::parse_equippable(std::istream &s) {
    auto new_equippable = make_shared<Equippable>();
    string name;
    s >> name;
    int id {parse_id(name)};
    new_equippable->types.insert(id);
    new_equippable->types.insert(parse_id("item"));
    new_equippable->types.insert(parse_id("equippable"));
    string command;
    while(s >> command, command != "end") {
        read_equippable_command(new_equippable, command, s);
    }
    state.lock()->library.add_equippable(id, new_equippable);
}

int Loader::parse_id(std::string name) {
    auto it = type_table.find(name);
    if (it == type_table.end()) {
        int id = next_id++;
        type_table.emplace(name, id);
        type_back_table.emplace(id, name);
        return id;
    }
    return it->second;
}

void Loader::load_file(std::string file) {
    parse_file(file);
}

void Loader::read_entity_command(const std::shared_ptr<Entity> &entity, const std::string &command, std::istream &s) {
    if (command == "set") {
        string set;
        s >> set;
        if (set_table.count(set)) {
            entity->add_feature_set(*set_table[set]);
            if (sets_is_type.count(parse_id(set))) {
                entity->types.insert(parse_id(set));
            }
        } else {
            cerr << "Could not find set \"" << set << "\"" << endl;
        }
    } else if (command == "representation") {
        char c;
        s >> c;
        entity->representation = c;
    } else if (command == "name") {
        entity->name = read_string(s);
    } else if (command == "score") {
        int score;
        s >> score;
        entity->score = score;
    } else if (command == "type") {
        string type;
        s >> type;
        entity->types.insert(parse_id(type));
    } else {
        cerr << "Unknown Command \"" << command << "\"" << endl;
    }
}

void Loader::read_mob_command(const std::shared_ptr<Character> &entity, const std::string &command, std::istream &s) {
    if (command == "controller") {
        entity->controller = load_controller(s);
    } else {
        read_entity_command(entity, command, s);
    }
}

void Loader::read_item_command(const std::shared_ptr<Item> &entity, const std::string &command, std::istream &s) {
    if (command == "value") {
        int value;
        s >> value;
        entity->value = value;
    } else {
        read_entity_command(entity, command, s);
    }
}

void Loader::read_consumable_command(const std::shared_ptr<Consumable> &entity, const std::string &command, std::istream &s) {
    if (command == "consumed") {
        int num_turns; // TODO check syntax error
        s >> num_turns;
        string set;
        s >> set;
        if (set_table.count(set)) {
            entity->set = set_table[set];
            entity->num_turns = num_turns;
        } else {
            cerr << "Could not find set \"" << set << "\"" << endl;
        }
    } else if (command == "effect_type"){
        string effect_type;
        s >> effect_type;
        if (effect_type == "positive") {
            entity->effect_type = POSITIVE;
        } else if (effect_type == "negative") {
            entity->effect_type = NEGATIVE;
        } else if (effect_type == "neutral") {
            entity->effect_type = NEUTRAL;
        } else {
            cerr << "Unknown effect_type \"" << effect_type << "\"" << endl;
        }
    } else {
        read_item_command(entity, command, s);
    }
}

void Loader::read_equippable_command(const std::shared_ptr<Equippable> &entity, const std::string &command, std::istream &s) {
    if (command == "equipped") {
        string slot;
        s >> slot;
        string set;
        s >> set;
        if (set_table.count(set)) {
            entity->slot = parse_id(slot);
            entity->set = set_table[set];
        } else {
            cerr << "Could not find set \"" << set << "\"" << endl;
        }
    } else {
        read_item_command(entity, command, s);
    }
}
