#pragma once

#include "Item.h"

class Equippable : public Item {
public:
    class Target : public EventTarget {
    public:
        Target(const std::shared_ptr<Equippable> &equippable);
        std::shared_ptr<Equippable> as_equippable() override;
        std::shared_ptr<Entity> as_entity() override;
        std::shared_ptr<Item> as_item() override;

    private:
        std::shared_ptr<Equippable> equippable;
    };

    Equippable(const Equippable&other);

    Equippable();

    Equippable(std::string name);

    void equip(const std::shared_ptr<Character> &onto) override;
    void unequip() override;
    bool equippable() override;
    int equipped_slot() override;

    int get_slot();

    std::shared_ptr<Entity> clone() override;

private:
    std::shared_ptr<FeatureSet> set;
    int slot {-1};

    std::shared_ptr<Character> equipped_to;

    friend class Loader;
};



