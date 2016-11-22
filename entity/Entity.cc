#include "Entity.h"

using namespace std;

unique_ptr<EventTarget> Entity::getAsTarget() {
    return make_unique<Target>(this);
}

int Entity::getSize() {
    return size.value;
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

int Entity::getDodge() {
    return dodge.value;
}

Position Entity::getPosition() {
    return position;
}

void Entity::interact(Character *source) {

}

void Entity::attack(Entity *source, int amount) {
    // trigger attacked event here
    trigger(ATTACKED, source);
    damage(amount);
    // trigger attacked_done event here
    trigger(ATTACKED_DONE, source);
}

void Entity::addListReference(std::list<Entity *> &list, std::list<Entity *>::iterator reference) {
    listReferences[&list] = reference;
}

void Entity::removeListReference(std::list<Entity *> &list) {
    listReferences.erase(&list);
}

Entity::~Entity() {
    removeFromContainers();
}

void Entity::removeFromContainers() {
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
    trigger(MOVED, source);
    move(position + delta);
    // Trigger moved_done event here
    trigger(MOVED_DONE, source);
}

void Entity::move(Position destination) {
    position = destination;
}

void Entity::addListener(Listener &listener) {
    for(EventType type:listener.listeningFor()) {
        listeners[type].insert(&listener);
    }
}

void Entity::removeListener(Listener &listener) {
    for(EventType type:listener.listeningFor()) {
        listeners[type].erase(&listener);
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
        destroy();
    }
}

void Entity::addModifier(StatModifier &modifier) {
    Stat *stat {getCorrespondingStat(modifier)};
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
    Stat *stat {getCorrespondingStat(modifier)};
    switch (modifier.type) {
        case StatModifier::BASE:
            stat->bases.erase(stat->bases.find(std::make_pair(modifier.priority, modifier.base)));
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

bool Entity::isA(int type) {
    return (bool) types.count(type);
}

void Entity::doTurn() {

}

void Entity::addTemporaryFeatureSet(FeatureSet featureSet, EffectType effectType, int numTurns) {

}

Entity *Entity::clone() {
    return nullptr;
}

void Entity::addAction(Action &action) {

}

void Entity::removeAction(Action &action) {

}

void Entity::kill(Entity *source) {
    destroy();
}

void Entity::destroy() {
    // Trigger destroy
    trigger(DESTROYED);
    delete this;
    // Trigger destroy_done
    trigger(DESTROYED_DONE);
}

Entity::Target::Target(Entity *entity) : entity{entity} {}

Entity *Entity::Target::asEntity() {
    return entity;
}

void Entity::trigger(EventType eventType) {
    EventInfo info;
    info.primary = getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, Entity *secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, std::vector<Entity *> secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (Entity * secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, Position position) {
    EventInfo info;
    info.primary = getAsTarget();
    info.eventType = eventType;
    info.eventPosition = position;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, Position position, Entity *secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    info.eventPosition = position;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, Position position, std::vector<Entity *> secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (Entity * secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    info.eventPosition = position;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, int integer) {
    EventInfo info;
    info.primary = getAsTarget();
    info.eventType = eventType;
    info.eventInteger = integer;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, int integer, Entity *secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    info.eventInteger = integer;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, int integer, std::vector<Entity *> secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (Entity * secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    info.eventInteger = integer;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, float num) {
    EventInfo info;
    info.primary = getAsTarget();
    info.eventType = eventType;
    info.eventFloat = num;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, float num, Entity *secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    info.eventFloat = num;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, float num, std::vector<Entity *> secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (Entity * secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    info.eventFloat = num;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, double num) {
    EventInfo info;
    info.primary = getAsTarget();
    info.eventType = eventType;
    info.eventDouble = num;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, double num, Entity *secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    info.eventDouble = num;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, double num, std::vector<Entity *> secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (Entity * secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    info.eventDouble = num;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}

Stat * Entity::getCorrespondingStat(StatModifier &modifier) {
    switch (modifier.stat) {
        case SIZE:
            return &size;
        case MAX_HEALTH:
            return &maxHealth;
        case INITIATIVE:
            return &initiative;
        case DEFENSE_STRENGTH:
            return &defenseStrength;
        case KNOCKBACK_RESIST:
            return &knockbackResist;
        case DODGE:
            return &dodge;
        default:
            return nullptr;
    }
}
