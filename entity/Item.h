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
        std::shared_ptr<Entity> as_entity() override;
        std::shared_ptr<Item> as_item() override;

    private:
        std::shared_ptr<Item> item;
    };

    std::unique_ptr<EventTarget> get_as_target() override;

    std::shared_ptr<Entity> clone() override;

    int get_value();

private:
    int value {0};

    class PickupOnInteract : public Listener {
    public:
        virtual void notify(EventInfo &info) override;

        static const std::vector<EventType> event_types;
        const std::vector<EventType> listening_for() const override;
    };
    static std::shared_ptr<PickupOnInteract> pickup_on_interact;

    friend class Loader;
};


