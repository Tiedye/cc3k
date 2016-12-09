#include "Library.h"

#include "../FeatureSet.h"

using namespace std;

void Library::add_race(int id, std::unique_ptr<Race> race) {
    types.emplace(id, RACE);
    races.push_back(move(race));
}

void Library::add_mob(int id, const shared_ptr<Character> &mob) {
    types.emplace(id, MOB);
    mobs.emplace(id, move(mob));
}

void Library::add_item(int id, const shared_ptr<Item> &item) {
    types.emplace(id, ITEM);
    items.emplace(id, move(item));
}

void Library::add_consumable(int id, const shared_ptr<Consumable> &consumable) {
    types.emplace(id, CONSUMABLE);
    consumables.emplace(id, move(consumable));
}

void Library::add_equippable(int id, const shared_ptr<Equippable> &equippable) {
    types.emplace(id, EQUIPPABLE);
    equippables.emplace(id, move(equippable));
}

std::shared_ptr<Character> Library::get_a_mob(int id) {
    return static_pointer_cast<Character>(mobs[id]->clone());
}

std::shared_ptr<Item> Library::get_an_item(int id) {
    if (items.count(id)) {
        return static_pointer_cast<Item>(items[id]->clone());
    }
    if (equippables.count(id)) {
        return static_pointer_cast<Item>(equippables[id]->clone());
    }
    return static_pointer_cast<Item>(consumables[id]->clone());
}

std::shared_ptr<Consumable> Library::get_a_consumable(int id) {
    return static_pointer_cast<Consumable>(consumables[id]->clone());
}

std::shared_ptr<Equippable> Library::get_an_equippable(int id) {
    return static_pointer_cast<Equippable>(equippables[id]->clone());
}

const std::vector<std::unique_ptr<Race>>& Library::get_races() {
    return races;
}

Library::ItemType Library::get_type(int id) {
    return types[id];
}
