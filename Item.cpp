#include "Item.h"

using namespace std;

Item::Target::Target(Item *item):item{item} {}

Entity* Item::Target::asEntity() {
    return item;
}

Item* Item::Target::asItem() {
    return item;
}

std::unique_ptr<EventTarget> Item::getAsTarget() {
    return make_unique<Target>(this);
}