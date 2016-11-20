#pragma once

#include "Entity.h"

class Item : public Entity {
public:
    class Target : public EventTarget {
    public:
        Target(Item *item);
        Entity * asEntity() override;
        Item * asItem() override;

    private:
        Item *item;
    };

    std::unique_ptr<EventTarget> getAsTarget() override;
};


