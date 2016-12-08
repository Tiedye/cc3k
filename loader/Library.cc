#include "Library.h"

#include "../FeatureSet.h"

using namespace std;

void Library::addRace(int id, std::unique_ptr<Race> race) {
    types.emplace(id, RACE);
    races.push_back(move(race));
}

void Library::addMob(int id, std::unique_ptr<Character> mob) {
    types.emplace(id, MOB);
    mobs.emplace(id, move(mob));
}

void Library::addItem(int id, std::unique_ptr<Item> item) {
    types.emplace(id, ITEM);
    items.emplace(id, move(item));
}

void Library::addConsumable(int id, std::unique_ptr<Consumable> consumable) {
    types.emplace(id, CONSUMABLE);
    consumables.emplace(id, move(consumable));
}

void Library::addEquippable(int id, std::unique_ptr<Equippable> equippable) {
    types.emplace(id, EQUIPPABLE);
    equippables.emplace(id, move(equippable));
}

std::shared_ptr<Character> Library::getAMob(int id) {
    return static_pointer_cast<Character>(mobs[id]->clone());
}

std::shared_ptr<Item> Library::getAnItem(int id) {
    if (items.count(id)) {
        return static_pointer_cast<Item>(items[id]->clone());
    }
    if (equippables.count(id)) {
        return static_pointer_cast<Item>(equippables[id]->clone());
    }
    if (consumables.count(id)) {
        return static_pointer_cast<Item>(consumables[id]->clone());
    }
}

std::shared_ptr<Consumable> Library::getAConsumable(int id) {
    return static_pointer_cast<Consumable>(consumables[id]->clone());
}

std::shared_ptr<Equippable> Library::getAnEquippable(int id) {
    return static_pointer_cast<Equippable>(equippables[id]->clone());
}

const std::vector<std::unique_ptr<Race>>& Library::getRaces() {
    return races;
}

Library::ItemType Library::getType(int id) {
    return types[id];
}
