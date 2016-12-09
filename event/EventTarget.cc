#include "EventTarget.h"

std::shared_ptr<Entity> EventTarget::as_entity() {
	return nullptr;
}

std::shared_ptr<Character> EventTarget::as_character() {
	return nullptr;
}

std::shared_ptr<Item> EventTarget::as_item() {
	return nullptr;
}

std::shared_ptr<Consumable> EventTarget::as_consumable() {
	return nullptr;
}

std::shared_ptr<Equippable> EventTarget::as_equippable() {
	return nullptr;
}
