#pragma once

#include <memory>

class Entity;
class Character;
class Item;
class Consumable;
class Equippable;

struct EventTarget {
	virtual ~EventTarget() = default;
	virtual std::shared_ptr<Entity> as_entity();
	virtual std::shared_ptr<Character> as_character();
	virtual std::shared_ptr<Item> as_item();
	virtual std::shared_ptr<Consumable> as_consumable();
	virtual std::shared_ptr<Equippable> as_equippable();
};
