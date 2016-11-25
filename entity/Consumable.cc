#include "Consumable.h"

#include "Character.h"

Consumable::Consumable() {}

void Consumable::consume(std::shared_ptr<Character> by) {
    trigger(CONSUMED, by);
    by->trigger(CONSUME, shared_from_this());
    if (set) by->addTemporaryFeatureSet(shared_from_this(), set, effectType, numTurns);
    by->trigger(CONSUMED_DONE, shared_from_this());
    trigger(CONSUMED_DONE, by);
}

std::shared_ptr<Consumable> Consumable::Target::asConsumable() {
    return consumable;
}

std::shared_ptr<Entity> Consumable::Target::asEntity() {
    return consumable;
}

std::shared_ptr<Item> Consumable::Target::asItem() {
    return consumable;
}

Consumable::Target::Target(const std::shared_ptr<Consumable> &consumable) : consumable(consumable) {}
