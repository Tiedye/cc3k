#include "Entity.h"
#include "../controller/Controller.h"
#include "../Game.h"
#include "../stage/Dungeon.h"

using namespace std;

unique_ptr<EventTarget> Entity::getAsTarget() {
    return make_unique<Target>(shared_from_this());
}

int Entity::getSize() {
    return size.value;
}
int Entity::getMaxHealth() {
    return maxHealth.value;
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

int Entity::getHealth() {
    return health;
}
Position Entity::getPosition() {
    return position;
}

void Entity::setHealth(int amount) {
    health = amount;
}

void Entity::addListReference(std::list<std::shared_ptr<Entity>> &list, std::list<std::shared_ptr<Entity>>::iterator reference) {
    listReferences[&list] = reference;
}

void Entity::removeListReference(std::list<std::shared_ptr<Entity>> &list) {
    listReferences.erase(&list);
}

void Entity::removeFromContainers() {
    for (auto it = listReferences.begin(); it != listReferences.end(); ++it) {
        it->first->erase(it->second);
    }
}

void Entity::create() {
    trigger(CREATED);
    health = maxHealth.value;
    tempFeatureSets.clear();
    trigger(CREATED_DONE);
}

void Entity::occupy(shared_ptr<Entity> &source) {
    trigger(OCCUPIED, source);
    trigger(OCCUPIED_DONE, source);
}

void Entity::interact(shared_ptr<Entity> source) {
    trigger(INTERACTED, source);
    trigger(INTERACTED_DONE, source);
}

void Entity::damage(shared_ptr<Entity> source, int amount) {
    // trigger attacked event here
    trigger(ATTACKED, amount, source);
    int damageDone = damage(amount);
    // trigger attacked_done event here
    trigger(ATTACKED_DONE, damageDone, source);
}

int Entity::damage(int damage) {
    int preDamage {health};
    health -= damage * 100 / (100 + defenseStrength.value);
    checkDead();
    return preDamage - health;
}

void Entity::heal(shared_ptr<Entity> source, int amount) {
    // trigger healed
    trigger(HEALED, amount, source);
    heal(amount);
    // trigger healed_done
    trigger(HEALED_DONE, amount, source);
}

void Entity::heal(int amount) {
    health += amount;
}

void Entity::move(shared_ptr<Entity> &source, int distance, Direction direction) {
    // Trigger moved event here
    trigger(MOVED, source);
    move(distance, direction);
    // Trigger moved_done event here
    trigger(MOVED_DONE, source);
}

void Entity::move(int distance, Direction direction) {
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
    move(position + delta);
}

void Entity::move(shared_ptr<Entity> &source, Position destination) {
    trigger(MOVED, destination, source);
    Position origin {position};
    move(destination);
    trigger(MOVED_DONE, origin, source);
}

void Entity::move(Position destination) {
    position = destination;
}

void Entity::addListener(shared_ptr<Listener> listener) {
    for(EventType type:listener->listeningFor()) {
        listeners[type].insert(&listener);
    }
}

void Entity::removeListener(shared_ptr<Listener> listener) {
    for(EventType type:listener->listeningFor()) {
        listeners[type].erase(&listener);
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
}

bool Entity::isA(int type) {
    return (bool) types.count(type);
}

void Entity::doTurn(Dungeon &dungeon) {
    checkTempFeatures();
    ++turnCount;
}

void Entity::checkTempFeatures() {
    while (tempFeatureSets.size() && tempFeatureSets.begin()->first == turnCount) {
        TempFeatureSet &tempFeatureSet = tempFeatureSets.begin()->second;
        removeFeatureSet(*tempFeatureSet.set, tempFeatureSet.modNumerator, tempFeatureSet.modDenominator);
        tempFeatureSets.erase(tempFeatureSets.begin());
    }
}

void Entity::addTemporaryFeatureSet(shared_ptr<Entity> source, const shared_ptr<FeatureSet> featureSet,
                                    EffectType effectType,
                                    int numTurns) {
    EventInfo::Data data;
    data.integer1 = 1;
    data.integer2 = 1;

    trigger(TEMP_SET_ADD, data);

    TempFeatureSet tempFeatureSet;
    tempFeatureSet.effectType = effectType;
    tempFeatureSet.modNumerator = data.integer1;
    tempFeatureSet.modDenominator = data.integer2;
    tempFeatureSet.set = featureSet;
    tempFeatureSet.source = source;

    tempFeatureSets.emplace(turnCount+numTurns, tempFeatureSet);

    addFeatureSet(*featureSet, 0, 0);
}

shared_ptr<Entity> Entity::clone() {
    return nullptr;
}

void Entity::addAction(shared_ptr<Action> action) {
    actions.insert(action);
}

void Entity::removeAction(shared_ptr<Action> action) {
    actions.erase(action);
}

void Entity::kill(shared_ptr<Entity> &source) {
    destroy();
}

void Entity::destroy() {
    // Trigger destroy
    trigger(DESTROYED);
    removeFromContainers();
    // Trigger destroy_done
    trigger(DESTROYED_DONE);
}

Entity::Target::Target(std::shared_ptr<Entity> &entity) : entity{entity} {}

shared_ptr<Entity> Entity::Target::asEntity() {
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
void Entity::trigger(EventType eventType, shared_ptr<Entity> secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, vector<shared_ptr<Entity>> &secondaries) {
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
void Entity::trigger(EventType eventType, Position position) {
    EventInfo info;
    info.primary = getAsTarget();
    info.eventType = eventType;
    info.eventPosition = position;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, Position position, shared_ptr<Entity> secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    info.eventPosition = position;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, Position position, vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
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
void Entity::trigger(EventType eventType, int integer, shared_ptr<Entity> secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    info.eventInteger = integer;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, int integer, vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
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
void Entity::trigger(EventType eventType, float num, shared_ptr<Entity> secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    info.eventFloat = num;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, float num, vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
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
void Entity::trigger(EventType eventType, double num, shared_ptr<Entity> secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    info.eventDouble = num;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, double num, vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    info.eventDouble = num;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, EventInfo::Data &reference) {
    EventInfo info;
    info.primary = getAsTarget();
    info.eventType = eventType;
    info.eventDataPointer = &reference;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, EventInfo::Data &reference, shared_ptr<Entity> secondary) {
    EventInfo info;
    info.primary = getAsTarget();
    info.secondary = secondary->getAsTarget();
    info.eventType = eventType;
    info.eventDataPointer = &reference;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}
void Entity::trigger(EventType eventType, EventInfo::Data &reference, vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info;
    info.primary = getAsTarget();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->getAsTarget());
    }
    info.eventType = eventType;
    info.eventDataPointer = &reference;
    for (auto listener: listeners[eventType]) {
        listener->notify(info);
    }
}

Stat & Entity::getCorrespondingStat(StatModifier &modifier) {
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
            return {};
    }
}

void Entity::give(std::shared_ptr<Item> item) {
    // overridden
}

void Entity::equip(std::shared_ptr<Character> onto) {
    // overridden
}

void Entity::unequip() {
    // overridden
}

void Entity::consume(std::shared_ptr<Character> by) {
    // overridden
}

