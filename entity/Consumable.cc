#include "Consumable.h"

#include "Character.h"

using namespace std;

Consumable::Consumable() : Item("") {}

Consumable::Consumable(std::string name) : Item(name) {

}

void Consumable::consume(const shared_ptr<Character> &by) {
    auto self = shared_from_base<Consumable>();
    trigger(CONSUMED, by);
    by->trigger(CONSUME, self);
    if (set) by->addTemporaryFeatureSet(shared_from_base<Consumable>(), set, effectType, numTurns);
    by->trigger(CONSUME_DONE, self);
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

Consumable::Target::Target(const shared_ptr<Consumable> &consumable) : consumable(consumable) {}


std::shared_ptr<Entity> Consumable::clone() {
    return make_shared<Consumable>(*this);
}

Consumable::Consumable(const Consumable &other):
        Item(other),
        effectType{other.effectType},
        set{other.set},
        numTurns{other.numTurns}{

}