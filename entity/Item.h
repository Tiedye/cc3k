#pragma once

#include "Entity.h"

#include "../event/Listener.h"

class Item : public Entity {
public:
    Item(const Item&other);

    Item();

    Item(std::string name);

    class Target : public EventTarget {
    public:
        Target(const std::shared_ptr<Item> &item);
        std::shared_ptr<Entity> asEntity() override;
        std::shared_ptr<Item> asItem() override;

    private:
        std::shared_ptr<Item> item;
    };

    std::unique_ptr<EventTarget> getAsTarget() override;

    std::shared_ptr<Entity> clone() override;

    int getValue();

private:
    int value {0};

    class PickupOnInteract : public Listener {
    public:
        virtual void notify(EventInfo &info) override;

        static const std::vector<EventType> eventTypes;
        const std::vector<EventType> listeningFor() const override;
    };
    static std::shared_ptr<PickupOnInteract> pickupOnInteract;

    friend class Loader;
};


