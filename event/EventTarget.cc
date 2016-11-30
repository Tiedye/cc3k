#include "EventTarget.h"

std::shared_ptr<Entity> EventTarget::asEntity() {
	return nullptr;
}

std::shared_ptr<Character> EventTarget::asCharacter() {
	return nullptr;
}

std::shared_ptr<Item> EventTarget::asItem() {
	return nullptr;
}

std::shared_ptr<Consumable> EventTarget::asConsumable() {
	return nullptr;
}

std::shared_ptr<Equippable> EventTarget::asEquippable() {
	return nullptr;
}
