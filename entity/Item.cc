#include "Item.h"

#include "Character.h"

using namespace std;

Item::Target::Target(const shared_ptr<Item> &item): item{item} {}

shared_ptr<Entity> Item::Target::asEntity() {
    return item;
}

std::shared_ptr<Item> Item::Target::asItem() {
    return item;
}

std::unique_ptr<EventTarget> Item::getAsTarget() {
    return make_unique<Target>(shared_from_base<Item>());
}

Item::Item() : Item("") {
}

void Item::PickupOnInteract::notify(EventInfo &info) {
    auto self = info.primary->asItem();
    auto other = info.secondary->asCharacter();
    EventInfo::Data data;
    data.integer1 = true;
    self->trigger(ADD_TO_INVENTORY, data, other);
    if (data.integer1) {
        self->removeFromContainers();
        other->give(info.primary->asItem());
        self->trigger(ADD_TO_INVENTORY_DONE, other);
    }
}

const std::vector<EventType> Item::PickupOnInteract::eventTypes {INTERACTED_DONE};

const std::vector<EventType> Item::PickupOnInteract::listeningFor() const {
    return eventTypes;
}

shared_ptr<Item::PickupOnInteract> Item::pickupOnInteract = make_shared<Item::PickupOnInteract>();

shared_ptr<Entity> Item::clone() {
    return make_shared<Item>(*this);
}

int Item::getValue() {
    return value;
}

Item::Item(const Item &other): Entity(other), value{other.value} {

}

Item::Item(string name) : Entity(name) {
    addListener(Item::pickupOnInteract);
}
