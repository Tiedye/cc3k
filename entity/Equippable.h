#pragma once

#include "Item.h"

class Equippable : public Item, std::enable_shared_from_this<Equippable> {
public:
    class Target : public EventTarget {
    public:
        Target(const std::shared_ptr<Equippable> &equippable);
        std::shared_ptr<Equippable> asEquippable() override;
        std::shared_ptr<Entity> asEntity() override;
        std::shared_ptr<Item> asItem() override;

    private:
        std::shared_ptr<Equippable> equippable;
    };

    Equippable();

    void equip(std::shared_ptr<Character> onto) override;
    void unequip() override;

private:
    std::shared_ptr<FeatureSet> set;
    int slot {-1};

    std::shared_ptr<Character> equippedTo;

    friend class Loader;
};



