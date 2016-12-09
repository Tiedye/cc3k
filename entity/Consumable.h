#pragma once


#include "Item.h"

class Consumable : public Item {
public:
    class Target : public EventTarget {
    public:
        Target(const std::shared_ptr<Consumable> &consumable);
        std::shared_ptr<Entity> as_entity() override;
        std::shared_ptr<Item> as_item() override;
        std::shared_ptr<Consumable> as_consumable() override;

    private:
        std::shared_ptr<Consumable> consumable;
    };

    Consumable(const Consumable&other);

    Consumable();

    Consumable(std::string name);

    void consume(const std::shared_ptr<Character> &by) override;
    bool consumable() override;

    std::shared_ptr<Entity> clone() override;

private:
    EffectType effect_type;
    std::shared_ptr<FeatureSet> set;
    int num_turns;

    friend class Loader;
};



