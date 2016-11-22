#pragma once

#include "Entity.h"

class Item : public Entity {
public:
    Item();

    class Target : public EventTarget {
    public:
        Target(Item *item);
        Entity * asEntity() override;
        Item * asItem() override;

    private:
        Item *item;
    };

    std::unique_ptr<EventTarget> getAsTarget() override;

private:
    class PickupOnInteract : public Listener {
    public:
        virtual void notify(EventInfo &info) override;

        static const std::vector<EventType> eventTypes;
        const std::vector<EventType> listeningFor() const override;
    };
    static PickupOnInteract pickupOnInteract;
};


