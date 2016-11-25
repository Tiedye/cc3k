#include "Item.h"
#include "Character.h"

using namespace std;

Item::Target::Target(shared_ptr<Item> &item): item{item} {}

shared_ptr<Entity> Item::Target::asEntity() {
    return item;
}

std::shared_ptr<Item> Item::Target::asItem() {
    return item;
}

std::unique_ptr<EventTarget> Item::getAsTarget() {
    return make_unique<Target>(shared_from_this());
}

Item::Item() {
    addListener(Item::pickupOnInteract);
}

void Item::PickupOnInteract::notify(EventInfo &info) {
    info.primary->asEntity()->removeFromContainers();
    info.secondary->asCharacter()->give(info.primary->asItem());
}

std::vector<EventType> Item::PickupOnInteract::eventTypes {INTERACTED_DONE};

const std::vector<EventType> Item::PickupOnInteract::listeningFor() const {
    return eventTypes;
}

shared_ptr<Item::PickupOnInteract> Item::pickupOnInteract = make_shared<Item::PickupOnInteract>();

shared_ptr<Entity> Item::clone() {
    return new Item(*this);
}

int Item::getValue() {
    return value;
}
