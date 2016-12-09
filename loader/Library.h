#pragma once

#include <memory>
#include <map>
#include <vector>
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

    void add_race(int id, std::unique_ptr<Race> race);

    void add_mob(int id, const std::shared_ptr<Character> &mob);
    void add_item(int id, const std::shared_ptr<Item> &item);
    void add_consumable(int id, const std::shared_ptr<Consumable> &consumable);
    void add_equippable(int id, const std::shared_ptr<Equippable> &equippable);

    const std::vector<std::unique_ptr<Race>>& get_races();

    std::shared_ptr<Character> get_a_mob(int id);
    std::shared_ptr<Item> get_an_item(int id);
    std::shared_ptr<Consumable> get_a_consumable(int id);
    std::shared_ptr<Equippable> get_an_equippable(int id);

    ItemType get_type(int id);

private:
    std::map<int, ItemType> types;

    std::vector<std::unique_ptr<Race>> races;

    std::map<int, std::shared_ptr<Character>> mobs;
    std::map<int, std::shared_ptr<Item>> items;
    std::map<int, std::shared_ptr<Consumable>> consumables;
    std::map<int, std::shared_ptr<Equippable>> equippables;
};



