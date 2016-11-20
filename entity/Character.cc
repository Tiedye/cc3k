#include "Character.h"

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
	return attackStrength;
}

int Character::getSpellStrength() {
	return spellStrength;
}

int Character::getSpeed() {
	return speed;
}

int Character::getTenacity() {
	return tenacity;
}

int Character::getAccuracy() {
    return accuracy;
}
