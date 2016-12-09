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
    if (set) by->add_temporary_feature_set(shared_from_base<Consumable>(), set, effect_type, num_turns);
    destroy();
    by->trigger(CONSUME_DONE, self);
    trigger(CONSUMED_DONE, by);
}

bool Consumable::consumable() {
    return true;
}

std::shared_ptr<Consumable> Consumable::Target::as_consumable() {
    return consumable;
}

std::shared_ptr<Entity> Consumable::Target::as_entity() {
    return consumable;
}

std::shared_ptr<Item> Consumable::Target::as_item() {
    return consumable;
}

Consumable::Target::Target(const shared_ptr<Consumable> &consumable) : consumable(consumable) {}


std::shared_ptr<Entity> Consumable::clone() {
    return make_shared<Consumable>(*this);
}

Consumable::Consumable(const Consumable &other):
        Item(other),
        effect_type{other.effect_type},
        set{other.set},
        num_turns{other.num_turns}{

}