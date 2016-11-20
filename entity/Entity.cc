#include "Entity.h"

using namespace std;

unique_ptr<EventTarget> Entity::getAsTarget() {
    return make_unique<Target>(this);
}

Size Entity::getSize() {
    return size;
}

int Entity::getMaxHealth() {
    return maxHealth.value;
}

int Entity::getHealth() {
    return health;
}

int Entity::getInitiative() {
    return initiative.value;
}

int Entity::getDefenceStrength() {
    return defenseStrength.value;
}

int Entity::getKnockbackResist() {
    return knockbackResist.value;
}

Position Entity::getPosition() {
    return position;
}

void Entity::interact(Character *source) {

}

void Entity::attack(Entity *source, int amount) {
    // trigger attacked event here
    damage(amount);
    // trigger attacked_done event here
}

void Entity::addListReference(std::list<Entity *> &list, std::list<Entity*>::iterator reference) {
    listReferences[&list] = reference;
}

void Entity::removeListReference(std::list<Entity *> &list) {
    listReferences.erase(&list);
}

Entity::~Entity() {
    for (auto it = listReferences.begin(); it != listReferences.end(); ++it) {
        it->first->erase(it->second);
    }
}

void Entity::damage(int damage) {
    health -= damage * 100 / (100 + defenseStrength.value);
}

void Entity::knock(Entity *source, int distance, Direction direction) {
    distance = knockbackResist.value >= distance ? 0 : distance - knockbackResist.value;
    Position delta;
    switch (direction) {
        case NW:
        case N:
        case NE:
            position.row = -distance;
            break;
        case SW:
        case S:
        case SE:
            position.row = distance;
            break;
        default:
            break;
    }
    switch (direction) {
        case NW:
        case W:
        case SW:
            position.col = -distance;
            break;
        case NE:
        case E:
        case SE:
            position.col = distance;
            break;
        default:
            break;
    }
    // Trigger moved event here
    move(position + delta);
    // Trigger moved_done event here
}

void Entity::move(Position destination) {
    position = destination;
}

void Entity::addListener(Listener &listener) {
    if (listener.isListeningFor(INTERACTED)) {
        interactedListeners.insert(&listener);
    }
    if (listener.isListeningFor(INTERACTED_DONE)) {
        interactedDoneListeners.insert(&listener);
    }
    if (listener.isListeningFor(ATTACKED)) {
        attackedListeners.insert(&listener);
    }
    if (listener.isListeningFor(ATTACKED_DONE)) {
        attackedDoneListeners.insert(&listener);
    }
    if (listener.isListeningFor(MOVED)) {
        movedListeners.insert(&listener);
    }
    if (listener.isListeningFor(MOVED_DONE)) {
        movedDoneListeners.insert(&listener);
    }
    if (listener.isListeningFor(OCCUPIED)) {
        occupiedListeners.insert(&listener);
    }
    if (listener.isListeningFor(OCCUPIED_DONE)) {
        occupiedDoneListeners.insert(&listener);
    }
    if (listener.isListeningFor(CREATED)) {
        createdListeners.insert(&listener);
    }
    if (listener.isListeningFor(CREATED_DONE)) {
        createdDoneListeners.insert(&listener);
    }
    if (listener.isListeningFor(DESTROYED)) {
        destroyedListeners.insert(&listener);
    }
    if (listener.isListeningFor(DESTROYED_DONE)) {
        destroyedDoneListeners.insert(&listener);
    }
}

void Entity::removeListener(Listener &listener) {
    if (listener.isListeningFor(INTERACTED)) {
        interactedListeners.erase(&listener);
    }
    if (listener.isListeningFor(INTERACTED_DONE)) {
        interactedDoneListeners.erase(&listener);
    }
    if (listener.isListeningFor(ATTACKED)) {
        attackedListeners.erase(&listener);
    }
    if (listener.isListeningFor(ATTACKED_DONE)) {
        attackedDoneListeners.erase(&listener);
    }
    if (listener.isListeningFor(MOVED)) {
        movedListeners.erase(&listener);
    }
    if (listener.isListeningFor(MOVED_DONE)) {
        movedDoneListeners.erase(&listener);
    }
    if (listener.isListeningFor(OCCUPIED)) {
        occupiedListeners.erase(&listener);
    }
    if (listener.isListeningFor(OCCUPIED_DONE)) {
        occupiedDoneListeners.erase(&listener);
    }
    if (listener.isListeningFor(CREATED)) {
        createdListeners.erase(&listener);
    }
    if (listener.isListeningFor(CREATED_DONE)) {
        createdDoneListeners.erase(&listener);
    }
    if (listener.isListeningFor(DESTROYED)) {
        destroyedListeners.erase(&listener);
    }
    if (listener.isListeningFor(DESTROYED_DONE)) {
        destroyedDoneListeners.erase(&listener);
    }
}

void Entity::addFeatureSet(FeatureSet &featureSet) {
    for (auto listener: featureSet.listeners) {
        addListener(*listener);
    }
    for (auto statModifier:featureSet.statModifiers) {
        addModifier(statModifier);
    }
    for (auto action: featureSet.actions) {
        addAction(*action);
    }
}

void Entity::removeFeatureSet(FeatureSet &featureSet) {
    for (auto listener: featureSet.listeners) {
        removeListener(*listener);
    }
    for (auto statModifier:featureSet.statModifiers) {
        removeModifier(statModifier);
    }
    for (auto action: featureSet.actions) {
        removeAction(*action);
    }
}

void Entity::checkDead() {
    if (health <= 0) {
        // Trigger destroy
        delete this;
        // Trigger destroy_done
    }
}

void Entity::addModifier(StatModifier &modifier) {
    Stat *stat;
    switch (modifier.stat) {
        case SIZE:
            stat = &size;
            break;
        case MAX_HEALTH:
            stat = &maxHealth;
            break;
        case INITIATIVE:
            stat = &initiative;
            break;
        case DEFENSE_STRENGTH:
            stat = &defenseStrength;
            break;
        case KNOCKBACK_RESIST:
            stat = &knockbackResist;
            break;
        default:
            return;
    }
    switch (modifier.type) {
        case StatModifier::BASE:
            stat->bases.emplace(modifier.priority, modifier.base);
            break;
        case StatModifier::ADD:
            stat->shift += modifier.amount;
            break;
        case StatModifier::SUBTRACT:
            stat->shift -= modifier.amount;
            break;
        case StatModifier::MULTIPLY:
            stat->multiplier *= modifier.amount;
            break;
        case StatModifier::DIVIDE:
            stat->divider *= modifier.amount;
            break;
        case StatModifier::MULTDIV:
            stat->multiplier *= modifier.numerator;
            stat->divider *= modifier.denominator;
            break;
        default:
            return;
    }
}

void Entity::removeModifier(StatModifier &modifier) {
    Stat *stat;
    switch (modifier.stat) {
        case SIZE:
            stat = &size;
            break;
        case MAX_HEALTH:
            stat = &maxHealth;
            break;
        case INITIATIVE:
            stat = &initiative;
            break;
        case DEFENSE_STRENGTH:
            stat = &defenseStrength;
            break;
        case KNOCKBACK_RESIST:
            stat = &knockbackResist;
            break;
        default:
            return;
    }
    switch (modifier.type) {
        case StatModifier::BASE:
            stat->bases.erase(std::make_pair(modifier.priority, modifier.base));
            break;
        case StatModifier::ADD:
            stat->shift -= modifier.amount;
            break;
        case StatModifier::SUBTRACT:
            stat->shift += modifier.amount;
            break;
        case StatModifier::MULTIPLY:
            stat->multiplier /= modifier.amount;
            break;
        case StatModifier::DIVIDE:
            stat->divider /= modifier.amount;
            break;
        case StatModifier::MULTDIV:
            stat->multiplier /= modifier.numerator;
            stat->divider /= modifier.denominator;
            break;
        default:
            return;
    }
}

Entity::Target::Target(Entity *entity) : entity{entity} {}

Entity *Entity::Target::asEntity() {
    return entity;
}
