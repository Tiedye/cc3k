#include "Character.h"

#include <algorithm>

#include "Item.h"
#include "Equippable.h"

#include "../controller/Controller.h"
#include "../stage/Dungeon.h"
#include "../util/StatModifier.h"

using namespace std;

Character::Target::Target(const shared_ptr<Character> &character) : character {character} {}

shared_ptr<Entity> Character::Target::asEntity() {
	return character;
}

std::shared_ptr<Character> Character::Target::asCharacter() {
	return character;
}

std::unique_ptr<EventTarget> Character::getAsTarget() {
	return make_unique<Target>( shared_from_base<Character>() );
}

int Character::getAttackStrength() {
	return attackStrength.value;
}

int Character::getSpellStrength() {
	return spellStrength.value;
}

int Character::getSpeed() {
	return speed.value;
}

int Character::getTenacity() {
	return tenacity.value;
}

int Character::getAccuracy() {
    return accuracy.value;
}

void Character::give(const shared_ptr<Item> &item) {
	trigger(REMOVED_FROM_FLOOR, item);
	inventory.push_front(item);
	item->addListReference(inventory, inventory.begin());
	trigger(REMOVED_FROM_FLOOR_DONE, item);
}

shared_ptr<Entity> Character::clone() {
	return make_shared<Character>(*this);
}

void Character::doTurn(Dungeon &dungeon) {
    const shared_ptr<Character> self = shared_from_base<Character>();
    EventInfo::Data shouldSkip;
    shouldSkip.integer1 = 0;
    trigger(TURN_START, shouldSkip, self);
    if (!shouldSkip.integer1) {
        trigger(TURN_START_DONE, self);
        if (controller) {
            vector<Controller::ActionAndRange> actionsAndRanges;
            for(auto action:actions) {
                // gets the possible targets of all actions,
                //  if an action targets inventory, it can be returned in the actionAndTarget struct see tag:INV
                actionsAndRanges.emplace_back();
                Controller::ActionAndRange &actionAndRange = actionsAndRanges.back();

                actionAndRange.range = dungeon.getTargetable(position, action->range, action->getMinRange(*this), action->getRange(*this), action->actionType == Action::MOVE);
                actionAndRange.action = action;
            }
            while (!actionsAndRanges.empty()) {
                Controller::ActionAndTarget actionAndTarget = controller->getAction(self, actionsAndRanges, dungeon.getState());

                if (actionAndTarget.action->type != Action::Step::PASSACTION) { // cant do more than one of an action type per turn
                    actionsAndRanges.erase(remove_if(actionsAndRanges.begin(), actionsAndRanges.end(), [&actionAndTarget](const Controller::ActionAndRange &obj){ return obj.action->type == actionAndTarget.action->type; }), actionsAndRanges.end());
                } else {
                    actionsAndRanges.clear(); // passactions dont have targets
                    continue;
                }

                auto action = actionAndTarget.action;
                auto targetPosition = actionAndTarget.target;

                vector<shared_ptr<Entity>> targets;

                if (actionAndTarget.targetEntity) {  // tag:INV
                    targets.push_back(actionAndTarget.targetEntity);
                } else {
                    targets = dungeon.getTargeted(self, targetPosition, action);
                }

                const Action::Type actionType = action->actionType;
                for(auto target:targets) {
                    switch (actionType) {
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

                            int amount{action->getAmount(*this)};
                            EventInfo::Data data;
                            data.integer1 = amount;
                            data.integer2 = true; // does it hit?

                            trigger(ATTACK, data, target);

                            bool miss = (rand() % 100) < getAccuracy();
                            bool dodge = (rand() % 100) < target->getDodge();
                            if (!data.integer2 || miss || dodge) {
                                trigger(MISS, target);
                                trigger(MISS_DONE, target);
                                trigger(ATTACK_DONE, 0, target);
                            } else {
                                target->damage(data.integer1, self);
                                trigger(ATTACK_DONE, data.integer1, target);
                            }

                            break;
                        }
                        case Action::AID: {
                            int amount {action->getAmount(*this)};
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
                    action->onUse(self, target, targetPosition);
                }

                if (actionType == Action::MOVE) {
                    EventInfo::Data data;
                    data.position = targetPosition;
                    trigger(MOVE, data);
                    move(data.position);
                    trigger(MOVE_DONE, data.position);
                    action->onUse(self, targetPosition);
                    for (auto& occupied:dungeon.getEntitiesAt(targetPosition)) {
                        occupied->occupy(self);
                    }
                    break;
                }
            }
        }
    }
    Entity::doTurn(dungeon); // manage things like turn events and temp effects
}

Stat & Character::getCorrespondingStat(const StatModifier &modifier) {
    switch (modifier.stat) {
        case ATTACK_STRENGTH:
            return attackStrength;
        case SPELL_STRENGTH:
            return spellStrength;
        case SPEED:
            return speed;
        case TENACITY:
            return tenacity;
        case ACCURACY:
            return accuracy;
        default:
            return Entity::getCorrespondingStat(modifier);
    }
}

void Character::equip(const shared_ptr<Equippable> &equippable) {
    trigger(EQUIP, equippable);
    if (slots[equippable->getSlot()]) {
        slots[equippable->getSlot()]->unequip();
    }
    slots[equippable->getSlot()] = equippable;
    trigger(EQUIP_DONE, equippable);
}

void Character::unequip(const shared_ptr<Equippable> &equippable) {
    trigger(UNEQUIP, equippable);
    slots[equippable->getSlot()] = nullptr;
    trigger(UNEQUIP_DONE, equippable);
}

Character::Character(const Character &other) :
        Entity(other),
        controller{other.controller},
        attackStrength{other.attackStrength},
        spellStrength{other.spellStrength},
        speed{other.speed},
        tenacity{other.tenacity},
        accuracy{other.accuracy},
        slots{other.slots},
        gold{other.gold} {
    for(auto item:other.inventory) {
        inventory.push_front(item);
        item->addListReference(inventory, inventory.begin());
    }

}

Character::Character() : Entity("") {

}

void Character::addGold(const int value) {
    gold += gold;
}

bool Character::removeGold(const int value) {
    if (value >= gold) {
        gold -= value;
        return true;
    } else {
        return false;
    }
}

int Character::currentGold() {
    return gold;
}

Character::Character(string name) : Entity(name) {

}
