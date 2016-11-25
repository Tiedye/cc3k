#pragma once


#include "../util/EffectType.h"
#include "Item.h"

class Consumable : public Item, std::enable_shared_from_this<Item> {
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


    Consumable();

    void consume(std::shared_ptr<Character> by) override;

private:
    EffectType effectType;
    std::shared_ptr<FeatureSet> set;
    int numTurns;

    friend class Loader;
};



