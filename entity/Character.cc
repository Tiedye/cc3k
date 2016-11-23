#include "Character.h"
#include "Item.h"

using namespace std;

Character::Target::Target( std::shared_ptr<Character> &character ) : character {character} {}

shared_ptr<Entity> Character::Target::asEntity() {
	return character;
}

std::shared_ptr<Character> Character::Target::asCharacter() {
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

void Character::give(std::shared_ptr<Item> item) {
    // Trigger pick_up
	trigger(PICK_UP, item);
	inventory.push_front(item);
    // next line is super type hacky, the reason fo this is c++ doesnt think list<Enity *> is compatible with list<Item *>
	item->addListReference(reinterpret_cast<list<std::shared_ptr<Entity>> &>(inventory),
                           reinterpret_cast<list<std::shared_ptr<Entity>> &>(inventory).begin());
    // Trigger pick_up_done
	trigger(PICK_UP_DONE, item);
}

void Character::addAction(Action &action) {
	Entity::addAction(action);
}

void Character::removeAction(Action &action) {
	Entity::removeAction(action);
}

shared_ptr<Entity> Character::clone() {
	return new Character(*this);
}

void Character::doTurn() {

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
