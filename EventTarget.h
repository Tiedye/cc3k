#pragma once

#include <memory>

class Entity;
class Character;
class Item;
class Consumable;
class Equipable;

struct EventTarget {
	virtual ~EventTarget() = default;
	virtual std::shared_ptr<Entity> asEntity();
	virtual std::shared_ptr<Character> asCharacter();
	virtual std::shared_ptr<Item> asItem();
	virtual std::shared_ptr<Consumable> asConsumable();
	virtual std::shared_ptr<Equipable> asEquipable();
};
