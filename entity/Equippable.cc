#include "Equippable.h"

#include "Character.h"

Equippable::Equippable() {}

Equippable::Target::Target(const std::shared_ptr<Equippable> &equippable) : equippable(equippable) {}

std::shared_ptr<Equippable> Equippable::Target::asEquippable() {
    return equippable;
}

std::shared_ptr<Entity> Equippable::Target::asEntity() {
    return equippable;
}

std::shared_ptr<Item> Equippable::Target::asItem() {
    return equippable;
}

void Equippable::equip(std::shared_ptr<Character> onto) {
    if (!equippedTo) {
        trigger(EQUIPPED, onto);
        if(set) onto->addFeatureSet(*set);
        onto->equip(shared_from_this());
        equippedTo = onto;
        trigger(EQUIPPED_DONE, onto);
    }
}

void Equippable::unequip() {
    if (equippedTo) {
        trigger(UNEQUIPPED, equippedTo);
        if (set) equippedTo->removeFeatureSet(*set);
        equippedTo->unequip(shared_from_this());
        equippedTo = nullptr;
        trigger(UNEQUIPPED_DONE, equippedTo);
    }
}
