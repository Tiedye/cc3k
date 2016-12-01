#include "Equippable.h"

#include "Character.h"

using namespace std;

Equippable::Equippable() : Item("") {}

Equippable::Equippable(std::string name) : Item(name) {

}

Equippable::Target::Target(const shared_ptr<Equippable> &equippable) : equippable(equippable) {}

std::shared_ptr<Equippable> Equippable::Target::asEquippable() {
    return equippable;
}

std::shared_ptr<Entity> Equippable::Target::asEntity() {
    return equippable;
}

std::shared_ptr<Item> Equippable::Target::asItem() {
    return equippable;
}

void Equippable::equip(const shared_ptr<Character> &onto) {
    if (!equippedTo) {
        trigger(EQUIPPED, onto);
        if(set) onto->addFeatureSet(*set);
        onto->equip(shared_from_base<Equippable>());
        equippedTo = onto;
        trigger(EQUIPPED_DONE, onto);
    }
}

void Equippable::unequip() {
    if (equippedTo) {
        trigger(UNEQUIPPED, equippedTo);
        if (set) equippedTo->removeFeatureSet(*set);
        equippedTo->unequip(shared_from_base<Equippable>());
        equippedTo = nullptr;
        trigger(UNEQUIPPED_DONE, equippedTo);
    }
}

int Equippable::getSlot() {
    return slot;
}

Equippable::Equippable(const Equippable &other):Item(other), set{other.set}, slot{other.slot} {

}

std::shared_ptr<Entity> Equippable::clone() {
    return make_shared<Equippable>(*this);
}