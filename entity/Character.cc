#include "Character.h"
#include "Item.h"

using namespace std;

Character::Target::Target( Character * character ) : character {character} {}

Entity * Character::Target::asEntity() {
	return character;
}

Character * Character::Target::asCharacter() {
	return character;
}

std::unique_ptr<EventTarget> Character::getAsTarget() {
	return make_unique<Target>( this );
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

void Character::give(Item *item) {
    // Trigger pick_up
	trigger(PICK_UP, item);
	inventory.push_front(item);
    // next line is super type hacky, the reason fo this is c++ doesnt think list<Enity *> is compatible with list<Item *>
	item->addListReference((list<Entity *> &) inventory, ((list<Entity *> &) inventory).begin());
    // Trigger pick_up_done
	trigger(PICK_UP_DONE, item);
}

void Character::addAction(Action &action) {
	Entity::addAction(action);
}

void Character::removeAction(Action &action) {
	Entity::removeAction(action);
}

Entity *Character::clone() {
	return new Character(*this);
}

void Character::doTurn() {
	Entity::doTurn();
}

Stat *Character::getCorrespondingStat(StatModifier &modifier) {
    switch (modifier.stat) {
        case ATTACK_STRENGTH:
            return &attackStrength;
        case SPELL_STRENGTH:
            return &spellStrength;
        case SPEED:
            return &speed;
        case TENACITY:
            return &tenacity;
        case ACCURACY:
            return &accuracy;
        default:
            return Entity::getCorrespondingStat(modifier);
    }
}
