#include "Equippable.h"

#include "Character.h"

using namespace std;

Equippable::Equippable() : Item("") {}

Equippable::Equippable(std::string name) : Item(name) {

}

Equippable::Target::Target(const shared_ptr<Equippable> &equippable) : equippable(equippable) {}

std::shared_ptr<Equippable> Equippable::Target::as_equippable() {
    return equippable;
}

std::shared_ptr<Entity> Equippable::Target::as_entity() {
    return equippable;
}

std::shared_ptr<Item> Equippable::Target::as_item() {
    return equippable;
}

void Equippable::equip(const shared_ptr<Character> &onto) {
    if (!equipped_to) {
        trigger(EQUIPPED, onto);
        if(set) onto->add_feature_set(*set);
        onto->equip(shared_from_base<Equippable>());
        equipped_to = onto;
        trigger(EQUIPPED_DONE, onto);
    }
}

void Equippable::unequip() {
    if (equipped_to) {
        trigger(UNEQUIPPED, equipped_to);
        if (set) equipped_to->remove_feature_set(*set);
        equipped_to->unequip(shared_from_base<Equippable>());
        trigger(UNEQUIPPED_DONE, equipped_to);
        equipped_to = nullptr;
    }
}

bool Equippable::equippable() {
    return true;
}

int Equippable::equipped_slot() {
    return equipped_to ? slot : -1;
}

int Equippable::get_slot() {
    return slot;
}

Equippable::Equippable(const Equippable &other):Item(other), set{other.set}, slot{other.slot} {

}

std::shared_ptr<Entity> Equippable::clone() {
    return make_shared<Equippable>(*this);
}