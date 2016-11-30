#pragma once

#include <memory>
#include <map>
#include "../Race.h"
#include "../entity/Entity.h"
#include "../entity/Character.h"
#include "../entity/Item.h"
#include "../entity/Consumable.h"
#include "../entity/Equippable.h"
class FeatureSet;

class Library {
public:

    enum ItemType {
        RACE,
        MOB,
        ITEM,
        CONSUMABLE,
        EQUIPPABLE
    };

    void addRace(int id, std::unique_ptr<Race> race);

    void addMob(int id, std::unique_ptr<Character> mob);
    void addItem(int id, std::unique_ptr<Item> item);
    void addConsumable(int id, std::unique_ptr<Consumable> consumable);
    void addEquippable(int id, std::unique_ptr<Equippable> equippable);

    const std::map<int, std::unique_ptr<Race>>& getRaces();
    std::shared_ptr<FeatureSet> &getRaceSet(int id);

    std::shared_ptr<Character> getAMob(int id);
    std::shared_ptr<Item> getAnItem(int id);
    std::shared_ptr<Consumable> getAConsumable(int id);
    std::shared_ptr<Equippable> getAnEquippable(int id);

    ItemType getType(int id);

private:
    std::map<int, ItemType> types;

    std::map<int, std::unique_ptr<Race>> races;

    std::map<int, std::unique_ptr<Character>> mobs;
    std::map<int, std::unique_ptr<Item>> items;
    std::map<int, std::unique_ptr<Consumable>> consumables;
    std::map<int, std::unique_ptr<Equippable>> equippables;
};



