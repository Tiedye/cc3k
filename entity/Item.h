#pragma once

#include "Entity.h"

class Item : public Entity {
public:
    Item();

    class Target : public EventTarget {
    public:
        Target(std::shared_ptr<Item> item);
        std::shared_ptr<Entity> asEntity() override;
        std::shared_ptr<Item> asItem() override;

    private:
        std::shared_ptr<Item> item;
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


