#include "Entity.h"

using namespace std;

void Entity::addFeature() {}

unique_ptr<EventTarget> Entity::getAsTarget() {
	return make_unique<Target>(this);
}

Size Entity::getSize() {
	return size;
}

int Entity::getMaxHealth() {
	return maxHealth;
}

int Entity::getHealth() {
	return health;
}

int Entity::getInitiative() {
	return initiative;
}

int Entity::getDefenceStrength() {
	return defenseStrength;
}

int Entity::getKnockBackResist() {
	return knockBackResist;
}

Entity::Target::Target( Entity * entity ) :entity {entity} {}

Entity * Entity::Target::asEntity() {
	return entity;
}
