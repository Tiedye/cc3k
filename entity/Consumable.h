#pragma once


#include "Item.h"

class Consumable : public Item {
public:
    class Target : public EventTarget {
    public:
        Target(const std::shared_ptr<Consumable> &consumable);
        std::shared_ptr<Entity> asEntity() override;
        std::shared_ptr<Item> asItem() override;
        std::shared_ptr<Consumable> asConsumable() override;

    private:
        std::shared_ptr<Consumable> consumable;
    };

    Consumable(const Consumable&other);

    Consumable();

    Consumable(std::string name);

    void consume(const std::shared_ptr<Character> &by) override;

    std::shared_ptr<Entity> clone() override;

private:
    EffectType effectType;
    std::shared_ptr<FeatureSet> set;
    int numTurns;

    friend class Loader;
};



