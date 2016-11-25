#include "Character.h"
#include "Item.h"
#include "Equippable.h"
#include "../controller/Controller.h"
#include "../Game.h"
#include "../stage/Dungeon.h"
#include <algorithm>

using namespace std;

Character::Target::Target( std::shared_ptr<Character> &character ) : character {character} {}

shared_ptr<Entity> Character::Target::asEntity() {
	return character;
}

std::shared_ptr<Character> Character::Target::asCharacter() {
	return character;
}

std::unique_ptr<EventTarget> Character::getAsTarget() {
	return make_unique<Target>( shared_from_this() );
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

void Character::give(std::shared_ptr<Item> item) {
	trigger(PICK_UP, item);
	inventory.push_front(item);
    // next line is super type hacky, the reason fo this is c++ doesnt think list<Enity *> is compatible with list<Item *>
	item->addListReference(reinterpret_cast<list<std::shared_ptr<Entity>> &>(inventory),
                           reinterpret_cast<list<std::shared_ptr<Entity>> &>(inventory).begin());
	trigger(PICK_UP_DONE, item);
}

void Character::addAction(shared_ptr<Action> action) {
	Entity::addAction(action);
}

void Character::removeAction(shared_ptr<Action> action) {
	Entity::removeAction(action);
}

shared_ptr<Entity> Character::clone() {
	return new Character(*this);
}

void Character::doTurn(Dungeon &dungeon) {
    if (controller) {
        vector<Controller::ActionAndRange> actionsAndRanges;
        for(auto action:actions) {
            actionsAndRanges.emplace_back();
            Controller::ActionAndRange &actionAndRange = actionsAndRanges.back();
            for(int range {0}; range < action->getRange(*this); ++range) {
                for(int y {position.row - range}; y < position.row + range; ++y) {
                    if (dungeon.getCellType({y, position.col + range}) != WALL) {
                        actionAndRange.range.emplace_back(y, position.col + range);
                    }
                }
                for(int x {position.col - range}; x < position.col + range; ++x) {
                    if (dungeon.getCellType({position.row + range, x}) != WALL) {
                        actionAndRange.range.emplace_back(position.row + range, x);
                    }
                }
                for(int y {position.row - range + 1}; y <= position.row + range; ++y) {
                    if (dungeon.getCellType({y, position.col - range}) != WALL) {
                        actionAndRange.range.emplace_back(y, position.col - range);
                    }
                }
                for(int x {position.col - range + 1}; x <= position.col + range; ++x) {
                    if (dungeon.getCellType({position.row - range, x}) != WALL) {
                        actionAndRange.range.emplace_back(position.row - range, x);
                    }
                }
            }
            actionAndRange.action = action;
        }
        while (!actionsAndRanges.empty()) {
            Controller::ActionAndTarget actionAndTarget = controller->getAction(shared_from_this(), actionsAndRanges);
            if (actionAndTarget.action->type == Action::Step::MOVE) {
                actionsAndRanges.erase(remove_if(actionsAndRanges.begin(), actionsAndRanges.end(), [](const Controller::ActionAndRange &obj){ return obj.action->type == Action::Step::MOVE; }), actionsAndRanges.end());
            } else if (actionAndTarget.action->type == Action::Step::ACTION) {
                actionsAndRanges.erase(remove_if(actionsAndRanges.begin(), actionsAndRanges.end(), [](const Controller::ActionAndRange &obj){ return obj.action->type == Action::Step::ACTION; }), actionsAndRanges.end());
            } else if (actionAndTarget.action->type == Action::Step::BONUS) {
                actionsAndRanges.erase(remove_if(actionsAndRanges.begin(), actionsAndRanges.end(), [](const Controller::ActionAndRange &obj){ return obj.action->type == Action::Step::BONUS; }), actionsAndRanges.end());
            } else {
                actionsAndRanges.clear();
            }
            auto action = actionAndTarget.action;
            Position &target = actionAndTarget.target;
            switch (action->actionType) {
                case Action::Type::AID:
                    switch (action->targets) {
                        case Action::Targets::SELF:
                            heal(action->getAmount(*this));
                            action->onUse(shared_from_this(), target);
                            break;
                        case Action::Targets::ALL:
                            if (action->aoe) {
                                // TODO aoe
                            } else {
                                std::shared_ptr<Entity> targetEntity = dungeon.getEntityAt(target);
                                targetEntity->heal(shared_from_this(), action->getAmount(*this));
                                action->onUse(shared_from_this(), targetEntity, target);
                            }
                            break;
                    }
                    break;
                case Action::Type::ATTACK:
                    switch (action->targets) {
                        case Action::Targets::ALL:
                            if (action->aoe) {
                                // TODO aoe
                            } else {
                                std::shared_ptr<Entity> targetEntity = dungeon.getEntityAt(target);
                                targetEntity->damage(shared_from_this(), action->getAmount(*this));
                                action->onUse(shared_from_this(), targetEntity, target);
                            }
                            break;
                    }
                    break;
                case Action::Type::CONSUME:
                    switch (action->targets) {
                        case Action::Targets::ALL:
                            if (action->aoe) {
                                // TODO aoe
                            } else {
                                std::shared_ptr<Entity> targetEntity = actionAndTarget.targetEntity ? actionAndTarget.targetEntity : dungeon.getEntityAt(target);
                                targetEntity->consume(shared_from_this());
                                action->onUse(shared_from_this(), targetEntity, target);
                            }
                            break;
                    }
                    break;
                case Action::Type::EFFECT:
                    switch (action->targets) {
                        case Action::Targets::SELF:
                            heal(action->getAmount(*this));
                            action->onUse(shared_from_this(), target);
                            break;
                        case Action::Targets::ALL:
                            if (action->aoe) {
                                // TODO aoe
                            } else {
                                std::shared_ptr<Entity> targetEntity = dungeon.getEntityAt(target);
                                action->onUse(shared_from_this(), targetEntity, target);
                            }
                            break;
                    }
                    break;
                case Action::Type::EQUIP:
                    switch (action->targets) {
                        case Action::Targets::ALL:
                            if (action->aoe) {
                                // TODO aoe
                            } else {
                                std::shared_ptr<Entity> targetEntity = actionAndTarget.targetEntity ? actionAndTarget.targetEntity : dungeon.getEntityAt(target);
                                targetEntity->equip(shared_from_this());
                                action->onUse(shared_from_this(), targetEntity, target);
                            }
                            break;
                    }
                    break;
                case Action::Type::UNEQUIP:
                    switch (action->targets) {
                        case Action::Targets::ALL:
                            if (action->aoe) {
                                // TODO aoe
                            } else {
                                std::shared_ptr<Entity> targetEntity = actionAndTarget.targetEntity ? actionAndTarget.targetEntity : dungeon.getEntityAt(target);
                                targetEntity->unequip();
                                action->onUse(shared_from_this(), targetEntity, target);
                            }
                            break;
                    }
                    break;
                case Action::Type::INTERACT:
                    switch (action->targets) {
                        case Action::Targets::ALL:
                            if (action->aoe) {
                                // TODO aoe
                            } else {
                                std::shared_ptr<Entity> targetEntity = actionAndTarget.targetEntity ? actionAndTarget.targetEntity : dungeon.getEntityAt(target);
                                targetEntity->interact(shared_from_this());
                                action->onUse(shared_from_this(), targetEntity, target);
                            }
                            break;
                    }
                    break;
                case Action::Type::MOVE:
                    move(target);
                    action->onUse(shared_from_this(), target);
                    break;
                case Action::Type::PASS:
                    break;
            }
        }
    }
    Entity::doTurn(dungeon);
}

Stat & Character::getCorrespondingStat(StatModifier &modifier) {
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

void Character::equip(shared_ptr<Equippable> equippable) {
    trigger(EQUIP, equippable);
    if (slots[equippable->slot]) {
        slots[equippable->slot]->unequip();
    }
    slots[equippable->slot] = equippable;
    trigger(EQUIP_DONE, equippable);
}

void Character::unequip(shared_ptr<Equippable> equippable) {
    trigger(UNEQUIP, equippable);
    slots[equippable->slot] = nullptr;
    trigger(UNEQUIP_DONE, equippable);
}
