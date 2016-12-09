#include "Character.h"

#include <algorithm>

#include "Item.h"
#include "Equippable.h"

#include "../controller/Controller.h"
#include "../stage/Dungeon.h"
#include "../util/StatModifier.h"
#include "../display/ConsoleDungeonIO.h"
#include "../action/PassAction.h"

using namespace std;

Character::Target::Target(const shared_ptr<Character> &character) : character {character} {}

shared_ptr<Entity> Character::Target::as_entity() {
	return character;
}

std::shared_ptr<Character> Character::Target::as_character() {
	return character;
}

std::unique_ptr<EventTarget> Character::get_as_target() {
	return make_unique<Target>( shared_from_base<Character>() );
}

int Character::get_attack_strength() const {
	return attack_strength.value;
}

int Character::get_spell_strength() const {
	return spell_strength.value;
}

int Character::get_speed() const {
	return speed.value;
}

int Character::get_accuracy() const {
    return accuracy.value;
}

void Character::give(const shared_ptr<Item> &item) {
	inventory.push_front(item);
	item->add_list_reference(inventory, inventory.begin());
}

shared_ptr<Entity> Character::clone() {
	return make_shared<Character>(*this);
}

void Character::do_turn(Dungeon &dungeon, const int turn_id) {
    const shared_ptr<Character> self = shared_from_base<Character>();
    EventInfo::Data should_skip;
    should_skip.integer1 = 0;
    trigger(TURN_START, should_skip, self);
    if (!should_skip.integer1) {
        trigger(TURN_START_DONE, self);
        if (controller) {
            vector<Controller::ActionAndRange> actions_and_ranges;
            for(auto action:actions) {
                // gets the possible targets of all actions,
                //  if an action targets inventory, it can be returned in the action_and_target struct see tag:INV
                actions_and_ranges.emplace_back();
                Controller::ActionAndRange &action_and_range = actions_and_ranges.back();

                action_and_range.range = dungeon.get_targetable(*this, action);
                action_and_range.action = action;
            }
            while (!actions_and_ranges.empty()) {

                actions_and_ranges.emplace_back();
                actions_and_ranges.back().action = make_shared<PassAction>();
                Controller::ActionAndTarget action_and_target = controller->get_action(self, actions_and_ranges, dungeon.get_state());
                actions_and_ranges.pop_back();

                if (action_and_target.action->type != Action::Step::PASSACTION) { // cant do more than one of an action type per turn
                    actions_and_ranges.erase(remove_if(actions_and_ranges.begin(), actions_and_ranges.end(), [&action_and_target](const Controller::ActionAndRange &obj){ return obj.action->type == action_and_target.action->type; }), actions_and_ranges.end());
                } else {
                    actions_and_ranges.clear(); // passactions dont have targets
                    continue;
                }

                auto action = action_and_target.action;
                auto target_position = action_and_target.target;

                vector<shared_ptr<Entity>> targets;

                if (action_and_target.target_entity) {  // tag:INV
                    targets.push_back(action_and_target.target_entity);
                } else {
                    targets = dungeon.get_targeted(self, target_position, action);
                }

                const Action::Type action_type = action->action_type;
                for(auto target:targets) {
                    switch (action_type) {
                        case Action::PASS: // pass and move are special
                        case Action::MOVE:
                            continue;
                        case Action::CONSUME: // everythin else triggers desired action
                            target->consume(self);
                            break;
                        case Action::EQUIP:
                            target->equip(self);
                            break;
                        case Action::UNEQUIP:
                            target->unequip();
                            break;
                        case Action::INTERACT:
                            target->interact(self);
                            break;
                        case Action::ATTACK: {
                            int amount{action->get_amount(*this)};
                            EventInfo::Data data;
                            data.integer1 = amount;
                            data.integer2 = true; // does it hit?

                            trigger(ATTACK, data, target);

                            bool hit = bernoulli_distribution(get_accuracy()/100.0)(dungeon.get_state()->gen);
                            bool dodge = bernoulli_distribution(target->get_dodge()/100.0)(dungeon.get_state()->gen);
                            if (!data.integer2 || !hit || dodge) {
                                trigger(MISS, target);
                                trigger(MISS_DONE, target);
                            } else {
                                int damage_done = target->damage(data.integer1, self);
                                if (damage_done == -1) {
                                    trigger(MISS, target);
                                    trigger(MISS_DONE, target);
                                } else {
                                    trigger(ATTACK_DONE, damage_done, target);
                                }
                            }

                            break;
                        }
                        case Action::AID: {
                            int amount {action->get_amount(*this)};
                            EventInfo::Data data;
                            data.integer1 = amount;
                            trigger(HEAL, data, target);
                            target->heal(data.integer1, self);
                            trigger(HEAL_DONE, data.integer1, target);
                            break;
                        }
                        case Action::EFFECT:
                            break;
                    }
                    action->on_use(self, target, target_position);
                }

                if (action_type == Action::MOVE) {
                    EventInfo::Data data;
                    auto old_pos = position;
                    data.position = target_position;
                    trigger(MOVE, data);
                    move(data.position);
                    trigger(MOVE_DONE, old_pos);
                    action->on_use(self, target_position);
                    for (auto& occupied:dungeon.get_entities_at(target_position)) {
                        occupied->occupy(self);
                    }
                    break;
                }
            }
        }
    }
    Entity::do_turn(dungeon, turn_id); // manage things like turn events and temp effects
}

Stat & Character::get_corresponding_stat(const StatModifier &modifier) {
    switch (modifier.stat) {
        case ATTACK_STRENGTH:
            return attack_strength;
        case SPELL_STRENGTH:
            return spell_strength;
        case SPEED:
            return speed;
        case TENACITY:
            return tenacity;
        case ACCURACY:
            return accuracy;
        default:
            return Entity::get_corresponding_stat(modifier);
    }
}

void Character::equip(const shared_ptr<Equippable> &equippable) {
    trigger(EQUIP, equippable);
    if (slots[equippable->get_slot()]) {
        slots[equippable->get_slot()]->unequip();
    }
    slots[equippable->get_slot()] = equippable;
    trigger(EQUIP_DONE, equippable);
}

void Character::unequip(const shared_ptr<Equippable> &equippable) {
    trigger(UNEQUIP, equippable);
    slots[equippable->get_slot()] = nullptr;
    trigger(UNEQUIP_DONE, equippable);
}

Character::Character(const Character &other) :
        Entity(other),
        controller{other.controller},
        attack_strength{other.attack_strength},
        spell_strength{other.spell_strength},
        speed{other.speed},
        accuracy{other.accuracy},
        slots{other.slots},
        gold{other.gold} {
    for(auto item:other.inventory) {
        inventory.push_front(item);
        item->add_list_reference(inventory, inventory.begin());
    }

}

Character::Character() : Entity("") {

}

void Character::add_gold(const int value) {
    gold += value;
}

bool Character::remove_gold(const int value) {
    if (value >= gold) {
        gold -= value;
        return true;
    } else {
        return false;
    }
}

int Character::current_gold() const {
    return gold;
}

Character::Character(string name) : Entity(name) {

}

const std::list<std::shared_ptr<Item>> &Character::get_inventory() const {
    return inventory;
}

void Character::clear_inventory() {
    for(auto& slot:slots) {
        if (slot.second) {
            slot.second->unequip();
        }
    }
    inventory.clear();
}
