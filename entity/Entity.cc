#include <iostream>
#include "Entity.h"

#include "Item.h"

#include "../controller/Controller.h"

#include "../event/Listener.h"
#include "../FeatureSet.h"
#include "../display/ConsoleDungeonIO.h"

using namespace std;

unique_ptr<EventTarget> Entity::get_as_target() {
    return make_unique<Target>(shared_from_this());
}

int Entity::get_size() const {
    return size.value;
}
int Entity::get_max_health() const {
    return max_health.value;
}
int Entity::get_initiative() const {
    return initiative.value;
}
int Entity::get_defence_strength() const {
    return defense_strength.value;
}
int Entity::get_knockback_resist() const {
    return knockback_resist.value;
}
int Entity::get_tenacity() const {
    return tenacity.value;
}
int Entity::get_dodge() const {
    return dodge.value;
}

int Entity::get_health() {
    return health;
}
Position Entity::get_position() const {
    return position;
}

void Entity::set_health(const int amount) {
    health = amount;
}

void Entity::start_tracking() {
    track_iterator_validity = true;
}

bool Entity::iterator_invalid() {
    if (track_iterator_validity) {
        track_iterator_validity = false;
        return false;
    } else {
        return true;
    }
}

void Entity::add_list_reference(std::list<std::shared_ptr<Entity>> &list, std::list<std::shared_ptr<Entity>>::iterator reference) {
    list_references.insert(make_pair(&list, reference));
}

void Entity::remove_list_reference(std::list<std::shared_ptr<Entity>> &list) {
    list_references.erase(&list);
}

void Entity::remove_from_containers() {
    track_iterator_validity = false;
    if (on_floor) {
        trigger(REMOVED_FROM_FLOOR);
    }
    for (auto it = list_references.begin(); it != list_references.end(); ++it) {
        it->first->erase(it->second);
    }
    list_references.clear();
    if (on_floor) {
        trigger(REMOVED_FROM_FLOOR_DONE);
        on_floor = false;
    }
}

void Entity::add_list_reference(std::list<std::shared_ptr<Item>> &list, std::list<std::shared_ptr<Item>>::iterator reference) {
    add_list_reference(reinterpret_cast<std::list<std::shared_ptr<Entity>> &>(list), reinterpret_cast<std::list<std::shared_ptr<Entity>>::iterator &>(reference));
    // cause because
}

void Entity::remove_list_reference(std::list<std::shared_ptr<Item>> &list) {
    remove_list_reference(reinterpret_cast<std::list<std::shared_ptr<Entity>> &>(list));
}

void Entity::create() {
    trigger(CREATED);
    dead = false;
    health = max_health.value;
    temp_feature_sets.clear();
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
        int damage_done = damage(data.integer1);
        // trigger attacked_done event here
        trigger(ATTACKED_DONE, damage_done, source);
        return damage_done;
    } else {
        return -1;
    }
}

int Entity::damage(const int damage) {
    int pre_damage {health};
    health -= damage * 100 / (100 + defense_strength.value);
    check_dead();
    return pre_damage - health;
}

int Entity::heal(const int amount, const shared_ptr<Entity> &source) {
    EventInfo::Data data;
    data.integer1 = amount;
    trigger(HEALED, data, source);
    int healing_done {heal(data.integer1)};
    trigger(HEALED_DONE, healing_done, source);
    return healing_done;
}

int Entity::heal(const int amount) {
    int initial_heath {health};
    health += amount;
    if (health > get_max_health()) health = get_max_health();
    return health - initial_heath;
}

void Entity::move(const int distance, const Direction direction, const shared_ptr<Entity> &source) {
    int calculated_distance = knockback_resist.value >= distance ? 0 : distance - knockback_resist.value;
    Position delta;
    switch (direction) {
        case NW:
        case N:
        case NE:
            position.y = -calculated_distance;
            break;
        case SW:
        case S:
        case SE:
            position.y = calculated_distance;
            break;
        default:
            break;
    }
    switch (direction) {
        case NW:
        case W:
        case SW:
            position.x = -calculated_distance;
            break;
        case NE:
        case E:
        case SE:
            position.x = calculated_distance;
            break;
        default:
            break;
    }
    move(position + delta, source);
}

void Entity::move(const int distance, const Direction direction) {
    int calculated_distance = knockback_resist.value >= distance ? 0 : distance - knockback_resist.value;
    Position delta;
    switch (direction) {
        case NW:
        case N:
        case NE:
            position.y = -calculated_distance;
            break;
        case SW:
        case S:
        case SE:
            position.y = calculated_distance;
            break;
        default:
            break;
    }
    switch (direction) {
        case NW:
        case W:
        case SW:
            position.x = -calculated_distance;
            break;
        case NE:
        case E:
        case SE:
            position.x = calculated_distance;
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

void Entity::add_listener(const shared_ptr<Listener> &listener) {
    for(EventType type:listener->listening_for()) {
        listeners[type].insert(listener);
    }
}

void Entity::remove_listener(const shared_ptr<Listener> &listener) {
    for(EventType type:listener->listening_for()) {
        listeners[type].erase(listener);
    }
}

void Entity::add_feature_set(const FeatureSet &feature_set, int mod_numerator, int mod_denominator) {
    for (auto listener: feature_set.listeners) {
        add_listener(listener);
    }
    for (auto stat_modifier:feature_set.stat_modifiers) {
        add_modifier(stat_modifier, mod_numerator, mod_denominator);
    }
    for (auto action: feature_set.actions) {
        add_action(action);
    }
}

void Entity::remove_feature_set(const FeatureSet &feature_set, int mod_numerator, int mod_denominator) {
    for (auto listener: feature_set.listeners) {
        remove_listener(listener);
    }
    for (auto stat_modifier:feature_set.stat_modifiers) {
        remove_modifier(stat_modifier, mod_numerator, mod_denominator);
    }
    for (auto action: feature_set.actions) {
        remove_action(action);
    }
}

void Entity::check_dead() {
    if (health <= 0) {
        destroy();
    }
}

void Entity::add_modifier(const StatModifier &modifier, int mod_numerator, int mod_denominator) {
    Stat &stat = get_corresponding_stat(modifier);
    switch (modifier.type) {
        case StatModifier::BASE:
            stat.bases.emplace(modifier.priority, modifier.base);
            break;
        case StatModifier::ADD:
            stat.shift += modifier.amount * mod_numerator / mod_denominator;
            break;
        case StatModifier::SUBTRACT:
            stat.shift -= modifier.amount * mod_numerator / mod_denominator;
            break;
        case StatModifier::MULTIPLY:
            stat.multiplier *= modifier.amount * mod_numerator / mod_denominator;
            break;
        case StatModifier::DIVIDE:
            stat.divider *= modifier.amount * mod_numerator / mod_denominator;
            break;
        case StatModifier::MULTDIV:
            stat.multiplier *= modifier.numerator * mod_numerator;
            stat.divider *= modifier.denominator * mod_denominator;
            break;
        default:
            return;
    }
    stat.update();
}

void Entity::remove_modifier(const StatModifier &modifier, int mod_numerator, int mod_denominator) {
    Stat &stat = get_corresponding_stat(modifier);
    switch (modifier.type) {
        case StatModifier::BASE:
            stat.bases.erase(stat.bases.find(std::make_pair(modifier.priority, modifier.base)));
            break;
        case StatModifier::ADD:
            stat.shift -= modifier.amount * mod_numerator / mod_denominator;
            break;
        case StatModifier::SUBTRACT:
            stat.shift += modifier.amount * mod_numerator / mod_denominator;
            break;
        case StatModifier::MULTIPLY:
            stat.multiplier /= modifier.amount * mod_numerator / mod_denominator;
            break;
        case StatModifier::DIVIDE:
            stat.divider /= modifier.amount * mod_numerator / mod_denominator;
            break;
        case StatModifier::MULTDIV:
            stat.multiplier /= modifier.numerator * mod_numerator;
            stat.divider /= modifier.denominator * mod_denominator;
            break;
        default:
            return;
    }
    stat.update();
}

bool Entity::is_a(int type) {
    return (bool) types.count(type);
}

void Entity::do_turn(Dungeon &dungeon, const int turn_id) {
    last_turn = turn_id;
    auto self = shared_from_this();
    trigger(TURN_END, self);
    check_temp_features();
    ++turn_count;
    trigger(TURN_END_DONE, self);
}

int Entity::last_turn_id() const {
    return last_turn;
}

void Entity::check_temp_features() {
    while (temp_feature_sets.size() && temp_feature_sets.begin()->first == turn_count) {
        TempFeatureSet &temp_feature_set = temp_feature_sets.begin()->second;
        remove_feature_set(*temp_feature_set.set, temp_feature_set.mod_numerator, temp_feature_set.mod_denominator);
        temp_feature_sets.erase(temp_feature_sets.begin());
    }
}

void Entity::add_temporary_feature_set(const shared_ptr<Entity> &source, const shared_ptr<FeatureSet> &feature_set,
                                    EffectType effect_type,
                                    int num_turns) {
    EventInfo::Data data;
    data.short1 = 1;
    data.short2 = 1;
    data.short3 = effect_type;

    trigger(TEMP_SET_ADD, data, source);

    effect_type = (EffectType)data.short3;

    TempFeatureSet temp_feature_set;
    temp_feature_set.effect_type = effect_type;
    temp_feature_set.mod_numerator = data.short1;
    temp_feature_set.mod_denominator = data.short2;
    temp_feature_set.set = feature_set;
    temp_feature_set.source = source;

    if (effect_type == EffectType::NEGATIVE) {
        num_turns = num_turns * (100 - get_tenacity()) / 100;
    }

    temp_feature_sets.emplace(turn_count+num_turns, temp_feature_set);

    add_feature_set(*feature_set, temp_feature_set.mod_numerator, temp_feature_set.mod_denominator);
}

void Entity::clear_temporary_feature_sets() {
    for(auto& temp_feature_set:temp_feature_sets) {
        remove_feature_set(*temp_feature_set.second.set, temp_feature_set.second.mod_numerator, temp_feature_set.second.mod_denominator);
    }
    temp_feature_sets.clear();
}

shared_ptr<Entity> Entity::clone() {
    return make_shared<Entity>(*this);
}

void Entity::add_action(const shared_ptr<Action> &action) {
    actions.insert(action);
}

void Entity::remove_action(const shared_ptr<Action> &action) {
    actions.erase(action);
}

void Entity::kill(const shared_ptr<Entity> &source) {
    destroy();
}

void Entity::destroy() {
    // Trigger destroy
    trigger(DESTROYED);
    dead = true;
    remove_from_containers();
    // Trigger destroy_done
    trigger(DESTROYED_DONE);
}

Entity::Target::Target(const shared_ptr<Entity> &entity) : entity{entity} {}

shared_ptr<Entity> Entity::Target::as_entity() {
    return entity;
}

void Entity::trigger(const EventType event_type) {
    EventInfo info;
    info.primary = get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const shared_ptr<Entity> &secondary) {
    EventInfo info;
    info.primary = get_as_target();
    info.secondary = secondary->get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info;
    info.primary = get_as_target();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->get_as_target());
    }
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const Position position) {
    EventInfo info(position);
    info.primary = get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const Position position, const shared_ptr<Entity> &secondary) {
    EventInfo info(position);
    info.primary = get_as_target();
    info.secondary = secondary->get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const Position position, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info;
    info.primary = get_as_target();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->get_as_target());
    }
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const int integer) {
    EventInfo info{integer,0};
    info.primary = get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const int integer, const shared_ptr<Entity> &secondary) {
    EventInfo info{integer,0};
    info.primary = get_as_target();
    info.secondary = secondary->get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const int integer, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info{integer,0};
    info.primary = get_as_target();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->get_as_target());
    }
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const float num) {
    EventInfo info{num};
    info.primary = get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const float num, const shared_ptr<Entity> &secondary) {
    EventInfo info{num};
    info.primary = get_as_target();
    info.secondary = secondary->get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const float num, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info{num};
    info.primary = get_as_target();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->get_as_target());
    }
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const double num) {
    EventInfo info{num};
    info.primary = get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const double num, const shared_ptr<Entity> &secondary) {
    EventInfo info{num};
    info.primary = get_as_target();
    info.secondary = secondary->get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, const double num, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info{num};
    info.primary = get_as_target();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->get_as_target());
    }
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, EventInfo::Data &reference) {
    EventInfo info{&reference};
    info.primary = get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, EventInfo::Data &reference, const shared_ptr<Entity> &secondary) {
    EventInfo info{&reference};
    info.primary = get_as_target();
    info.secondary = secondary->get_as_target();
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}
void Entity::trigger(const EventType event_type, EventInfo::Data &reference, const vector<shared_ptr<Entity>> &secondaries) {
    EventInfo info{&reference};
    info.primary = get_as_target();
    for (auto secondary: secondaries) {
        info.secondaries.push_back(secondary->get_as_target());
    }
    info.event_type = event_type;
    for (auto listener: listeners[event_type]) {
        listener->notify(info);
    }
}

Stat & Entity::get_corresponding_stat(const StatModifier &modifier) {
    switch (modifier.stat) {
        case SIZE:
            return size;
        case MAX_HEALTH:
            return max_health;
        case INITIATIVE:
            return initiative;
        case DEFENSE_STRENGTH:
            return defense_strength;
        case KNOCKBACK_RESIST:
            return knockback_resist;
        case DODGE:
            return dodge;
        default:
            return max_health; // because its got to return something...
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

int Entity::equipped_slot() {
    return -1;
}

const std::string &Entity::get_name() {
    return name;
}

Entity::Entity(const Entity &other) :
        representation{other.representation},
        name{other.name},
        position{other.position},
        health{other.health},
        size{other.size},
        max_health{other.max_health},
        initiative{other.initiative},
        defense_strength{other.defense_strength},
        knockback_resist{other.knockback_resist},
        tenacity{other.tenacity},
        dodge{other.dodge},
        actions{other.actions},
        turn_count{other.turn_count},
        listeners{other.listeners},
        temp_feature_sets{other.temp_feature_sets},
        types{other.types},
        score{other.score}{

}

Entity::Entity(std::string name) : name{name} {

}

bool Entity::is_dead() {
    return dead;
}

void Entity::make_a(int type) {
    types.insert(type);
}

void Entity::add_score(const int value) {
    score += value;
}

int Entity::current_score() const {
    return score;
}

void Entity::set_score(const int value) {
    score = value;
}

bool Entity::ListenerSort::operator()(const std::shared_ptr<Listener> &a, const std::shared_ptr<Listener> &b) const {
    return a->priority == b->priority ? a < b: a->priority < b->priority;
}
