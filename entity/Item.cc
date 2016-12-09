#include "Item.h"

#include "Character.h"

using namespace std;

Item::Target::Target(const shared_ptr<Item> &item): item{item} {}

shared_ptr<Entity> Item::Target::as_entity() {
    return item;
}

std::shared_ptr<Item> Item::Target::as_item() {
    return item;
}

std::unique_ptr<EventTarget> Item::get_as_target() {
    return make_unique<Target>(shared_from_base<Item>());
}

Item::Item() : Item("") {
}

void Item::PickupOnInteract::notify(EventInfo &info) {
    auto self = info.primary->as_item();
    auto other = info.secondary->as_character();
    EventInfo::Data data;
    data.integer1 = true;
    self->trigger(ADD_TO_INVENTORY, data, other);
    if (data.integer1) {
        self->remove_from_containers();
        other->give(info.primary->as_item());
        self->trigger(ADD_TO_INVENTORY_DONE, other);
    }
}

const std::vector<EventType> Item::PickupOnInteract::event_types {INTERACTED_DONE};

const std::vector<EventType> Item::PickupOnInteract::listening_for() const {
    return event_types;
}

shared_ptr<Item::PickupOnInteract> Item::pickup_on_interact = make_shared<Item::PickupOnInteract>();

shared_ptr<Entity> Item::clone() {
    return make_shared<Item>(*this);
}

int Item::get_value() {
    return value;
}

Item::Item(const Item &other): Entity(other), value{other.value} {

}

Item::Item(string name) : Entity(name) {
    add_listener(Item::pickup_on_interact);
}
