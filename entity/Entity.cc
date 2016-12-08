#include <iostream>
#include "Entity.h"

#include "Item.h"

#include "../controller/Controller.h"

#include "../event/Listener.h"
#include "../FeatureSet.h"
#include "../display/ConsoleDungeonIO.h"

using namespace std;

unique_ptr<EventTarget> Entity::getAsTarget() {
    return make_unique<Target>(shared_from_this());
}

int Entity::getSize() const {
    return size.value;
}
int Entity::getMaxHealth() const {
    return maxHealth.value;
}
int Entity::getInitiative() const {
    return initiative.value;
}
int Entity::getDefenceStrength() const {
    return defenseStrength.value;
}
int Entity::getKnockbackResist() const {
    return knockbackResist.value;
}
int Entity::getTenacity() const {
    return tenacity.value;
}
int Entity::getDodge() const {
    return dodge.value;
}

int Entity::getHealth() {
    return health;
}
Position Entity::getPosition() const {
    return position;
}

void Entity::setHealth(const int amount) {
    health = amount;
}

void Entity::startTracking() {
    trackIteratorValidity = true;
}

bool Entity::iteratorInvalid() {
    if (trackIteratorValidity) {
        trackIteratorValidity = false;
        return false;
    } else {
        return true;
    }
}

void Entity::addListReference(std::list<std::shared_ptr<Entity>> &list, std::list<std::shared_ptr<Entity>>::iterator reference) {
    listReferences.insert(make_pair(&list, reference));
}

void Entity::removeListReference(std::list<std::shared_ptr<Entity>> &list) {
    listReferences.erase(&list);
}

void Entity::removeFromContainers() {
    trackIteratorValidity = false;
    if (onFloor) {
        trigger(REMOVED_FROM_FLOOR);
    }
    for (auto it = listReferences.begin(); it != listReferences.end(); ++it) {
        it->first->erase(it->second);
    }
    listReferences.clear();
    if (onFloor) {
        trigger(REMOVED_FROM_FLOOR_DONE);
        onFloor = false;
    }
}

void Entity::addListReference(std::list<std::shared_ptr<Item>> &list, std::list<std::shared_ptr<Item>>::iterator reference) {
    addListReference(reinterpret_cast<std::list<std::shared_ptr<Entity>> &>(list), reinterpret_cast<std::list<std::shared_ptr<Entity>>::iterator &>(reference));
    // cause because
}

void Entity::removeListReference(std::list<std::shared_ptr<Item>> &list) {
    removeListReference(reinterpret_cast<std::list<std::shared_ptr<Entity>> &>(list));
}

void Entity::create() {
    trigger(CREATED);
    dead = false;
    health = maxHealth.value;
    tempFeatureSets.clear();
    trigger(CREATED_DONE);
}

void Entity::occupy(const shared_ptr<Entity> &source) {
    trigger(OCCUPIED, source);
    trigger(OCCUPIED_DONE, source);
}

void Entity::interact(const shared_ptr<Entity> &source) {
    trigger(INTERACTED, source);
    trigger(INTERACTED_DONE, source);
}

int Entity::damage(const int amount, const shared_ptr<Entity> &source) {
    // trigger attacked event here
    EventInfo::Data data;
    data.integer1 = amount;
    data.integer2 = true;
    trigger(ATTACKED, data, source);
    if (data.integer2) {
        int damageDone = damage(data.integer1);
        // trigger attacked_done event here
        trigger(ATTACKED_DONE, damageDone, source);
        return damageDone;
    } else {
        return -1;
    }
}

int Entity::damage(const int damage) {
    int preDamage {health};
    health -= damage * 100 / (100 + defenseStrength.value);
    checkDead();
    return preDamage - health;
}

int Entity::heal(const int amount, const shared_ptr<Entity> &source) {
    EventInfo::Data data;
    data.integer1 = amount;
    trigger(HEALED, data, source);
    int healingDone {heal(data.integer1)};
    trigger(HEALED_DONE, healingDone, source);
    return healingDone;
}

int Entity::heal(const int amount) {
    int initialHeath {health};
    health += amount;
    if (health > getMaxHealth()) health = getMaxHealth();
    return health - initialHeath;
}

void Entity::move(const int distance, const Direction direction, const shared_ptr<Entity> &source) {
    int calculatedDistance = knockbackResist.value >= distance ? 0 : distance - knockbackResist.value;
    Position delta;
    switch (direction) {
        case NW:
        case N:
        case NE:
            position.y = -calculatedDistance;
            break;
        case SW:
        case S:
        case SE:
            position.y = calculatedDistance;
            break;
        default:
            break;
    }
    switch (direction) {
        case NW:
        case W:
        case SW:
            position.x = -calculatedDistance;
            break;
        case NE:
        case E:
        case SE:
            position.x = calculatedDistance;
            break;
        default:
            break;
    }
    move(position + delta, source);
}

void Entity::move(const int distance, const Direction direction) {
    int calculatedDistance = knockbackResist.value >= distance ? 0 : distance - knockbackResist.value;
    Position delta;
    switch (direction) {
        case NW:
        case N:
        case NE:
            position.y = -calculatedDistance;
            break;
        case SW:
        case S:
        case SE:
            position.y = calculatedDistance;
            break;
        default:
            break;
    }
    switch (direction) {
        case NW:
        case W:
        case SW:
            position.x = -calculatedDistance;
            break;
        case NE:
        case E:
        case SE:
            position.x = calculatedDistance;
            break;
        default:
            break;
    }
    move(position + delta);
}

void Entity::move(const Position destination, const shared_ptr<Entity> &source) {
    EventInfo::Data data;
    data.position = destination;
    trigger(MOVED, data, source);
    Position origin {position};
    move(data.position);
    trigger(MOVED_DONE, origin, source);
}

void Entity::move(const Position destination) {
    position = destination;
}

void Entity::addListener(const shared_ptr<Listener> &listener) {
    for(EventType type:listener->listeningFor()) {
        listeners[type].insert(listener);
    }
}

void Entity::removeListener(const shared_ptr<Listener> &listener) {
    for(EventType type:listener->listeningFor()) {
        listeners[type].erase(listener);
    }
}

void Entity::addFeatureSet(const FeatureSet &featureSet, int modNumerator, int modDenominator) {
    for (auto listener: featureSet.listeners) {
        addListener(listener);
    }
    for (auto statModifier:featureSet.statModifiers) {
        addModifier(statModifier, modNumerator, modDenominator);
    }
    for (auto action: featureSet.actions) {
        addAction(action);
    }
}

void Entity::removeFeatureSet(const FeatureSet &featureSet, int modNumerator, int modDenominator) {
    for (auto listener: featureSet.listeners) {
        removeListener(listener);
    }
    for (auto statModifier:featureSet.statModifiers) {
        removeModifier(statModifier, modNumerator, modDenominator);
    }
    for (auto action: featureSet.actions) {
        removeAction(action);
    }
}

void Entity::checkDead() {
    if (health <= 0) {
        destroy();
    }
}

void Entity::addModifier(const StatModifier &modifier, int modNumerator, int modDenominator) {
    Stat &stat = getCorrespondingStat(modifier);
    switch (modifier.type) {
        case StatModifier::BASE:
            stat.bases.emplace(modifier.priority, modifier.base);
            break;
        case StatModifier::ADD:
            stat.shift += modifier.amount * modNumerator / modDenominator;
            break;
        case StatModifier::SUBTRACT:
            stat.shift -= modifier.amount * modNumerator / modDenominator;
            break;
        case StatModifier::MULTIPLY:
            stat.multiplier *= modifier.amount * modNumerator / modDenominator;
            break;
        case StatModifier::DIVIDE:
            stat.divider *= modifier.amount * modNumerator / modDenominator;
            break;
        case StatModifier::MULTDIV:
            stat.multiplier *= modifier.numerator * modNumerator;
            stat.divider *= modifier.denominator * modDenominator;
            break;
        default:
            return;
    }
    stat.update();
}

void Entity::removeModifier(const StatModifier &modifier, int modNumerator, int modDenominator) {
    Stat &stat = getCorrespondingStat(modifier);
    switch (modifier.type) {
        case StatModifier::BASE:
            stat.bases.erase(stat.bases.find(std::make_pair(modifier.priority, modifier.base)));
            break;
        case StatModifier::ADD:
            stat.shift -= modifier.amount * modNumerator / modDenominator;
            break;
        case StatModifier::SUBTRACT:
            stat.shift += modifier.amount * modNumerator / modDenominator;
            break;
        case StatModifier::MULTIPLY:
            stat.multiplier /= modifier.amount * modNumerator / modDenominator;
            break;
        case StatModifier::DIVIDE:
            stat.divider /= modifier.amount * modNumerator / modDenominator;
            break;
        case StatModifier::MULTDIV:
            stat.multiplier /= modifier.numerator * modNumerator;
            stat.divider /= modifier.denominator * modDenominator;
            break;
        default:
            return;
    }
    stat.update();
}

bool Entity::isA(int type) {
    return (bool) types.count(type);
}

void Entity::doTurn(Dungeon &dungeon, const int turnId) {
    lastTurn = turnId;
    auto self = shared_from_this();
    trigger(TURN_END, self);
    checkTempFeatures();
    ++turnCount;
    trigger(TURN_END_DONE, self);
}

int Entity::lastTurnId() const {
    return lastTurn;
}

void Entity::checkTempFeatures() {
    while (tempFeatureSets.size() && tempFeatureSets.begin()->first == turnCount) {
        TempFeatureSet &tempFeatureSet = tempFeatureSets.begin()->second;
        removeFeatureSet(*tempFeatureSet.set, tempFeatureSet.modNumerator, tempFeatureSet.modDenominator);
        tempFeatureSets.erase(tempFeatureSets.begin());
    }
}

void Entity::addTemporaryFeatureSet(const shared_ptr<Entity> &source, const shared_ptr<FeatureSet> &featureSet,
                                    EffectType effectType,
                                    int numTurns) {
    EventInfo::Data data;
    data.short1 = 1;
    data.short2 = 1;
    data.short3 = effectType;

    trigger(TEMP_SET_ADD, data, source);

    effectType = (EffectType)data.short3;

    TempFeatureSet tempFeatureSet;
    tempFeatureSet.effectType = effectType;
    tempFeatureSet.modNumerator = data.short1;
    tempFeatureSet.modDenominator = data.short2;
    tempFeatureSet.set = featureSet;
    tempFeatureSet.source = source;

    if (effectType == EffectType::NEGATIVE) {
        numTurns = numTurns * (100 - getTenacity()) / 100;
    }

    tempFeatureSets.emplace(turnCount+numTurns, tempFeatureSet);

    addFeatureSet(*featureSet, tempFeatureSet.modNumerator, tempFeatureSet.modDenominator);
}

void Entity::clearTemporaryFeatureSets() {
    for(auto& tempFeatureSet:tempFeatureSets) {
        removeFeatureSet(*tempFeatureSet.second.set, tempFeatureSet.second.modNumerator, tempFeatureSet.second.modDenominator);
    }
    tempFeatureSets.clear();
}

shared_ptr<Entity> Entity::clone() {
    return make_shared<Entity>(*this);
}

void Entity::addAction(const shared_ptr<Action> &action) {
    actions.insert(action);
}

void Entity::removeAction(const shared_ptr<Action> &action) {
    actions.erase(action);
}

void Entity::kill(const shared_ptr<Entity> &source) {
    destroy();
}

void Entity::destroy() {
    // Trigger destroy
    trigger(DESTROYED);
    dead = true;
    removeFromContainers();
    // Trigger destroy_done
    trigger(DESTROYED_DONE);
}

Entity::Target::Target(const shared_ptr<Entity> &entity) : entity{entity} {}

shared_ptr<Entity> Entity::Target::asEntity() {
    return entity;
}

void Entity::trigger(const EventType eventType) {
    EventInfo info;
    info.primary = getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const shared_ptr<Entity> &secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const Position position) {
    EventInfo info(position);
    info.primary = getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const Position position, const shared_ptr<Entity> &secondary) {
    EventInfo info(position);
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const Position position, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const int integer) {
    EventInfo info{integer,0};
    info.primary = getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const int integer, const shared_ptr<Entity> &secondary) {
    EventInfo info{integer,0};
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const int integer, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info{integer,0};
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const float num) {
    EventInfo info{num};
    info.primary = getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const float num, const shared_ptr<Entity> &secondary) {
    EventInfo info{num};
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const float num, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info{num};
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const double num) {
    EventInfo info{num};
    info.primary = getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const double num, const shared_ptr<Entity> &secondary) {
    EventInfo info{num};
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, const double num, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info{num};
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, EventInfo::Data &reference) {
    EventInfo info{&reference};
    info.primary = getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, EventInfo::Data &reference, const shared_ptr<Entity> &secondary) {
    EventInfo info{&reference};
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType eventType, EventInfo::Data &reference, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info{&reference};
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}

Stat & Entity::getCorrespondingStat(const StatModifier &modifier) {
    switch (modifier.stat) {
        case SIZE:
            return size;
        case MAX_HEALTH:
            return maxHealth;
        case INITIATIVE:
            return initiative;
        case DEFENSE_STRENGTH:
            return defenseStrength;
        case KNOCKBACK_RESIST:
            return knockbackResist;
        case DODGE:
            return dodge;
        default:
            return maxHealth; // because its got to return something...
    }
}

void Entity::give(const shared_ptr<Item> &item) {
    // overridden
}

void Entity::equip(const shared_ptr<Character> &onto) {
    // overridden
}

void Entity::unequip() {
    // overridden
}

void Entity::consume(const shared_ptr<Character> &by) {
    // overridden
}

bool Entity::consumable() {
    return false;
}

bool Entity::equippable() {
    return false;
}

int Entity::equippedSlot() {
    return -1;
}

const std::string &Entity::getName() {
    return name;
}

Entity::Entity(const Entity &other) :
        representation{other.representation},
        name{other.name},
        position{other.position},
        health{other.health},
        size{other.size},
        maxHealth{other.maxHealth},
        initiative{other.initiative},
        defenseStrength{other.defenseStrength},
        knockbackResist{other.knockbackResist},
        tenacity{other.tenacity},
        dodge{other.dodge},
        actions{other.actions},
        turnCount{other.turnCount},
        listeners{other.listeners},
        tempFeatureSets{other.tempFeatureSets},
        types{other.types},
        score{other.score}{

}

Entity::Entity(std::string name) : name{name} {

}

bool Entity::isDead() {
    return dead;
}

void Entity::makeA(int type) {
    types.insert(type);
}

void Entity::addScore(const int value) {
    score += value;
}

int Entity::currentScore() const {
    return score;
}

void Entity::setScore(const int value) {
    score = value;
}

bool Entity::ListenerSort::operator()(const std::shared_ptr<Listener> &a, const std::shared_ptr<Listener> &b) const {
    return a->priority == b->priority ? a < b: a->priority < b->priority;
}
