#pragma once

class Entity;
class Character;
class Item;
class Consumable;
class Equipable;

struct EventTarget {
	virtual ~EventTarget() = default;
	virtual Entity * asEntity();
	virtual Character * asCharacter();
	virtual Item * asItem();
	virtual Consumable * asConsumable();
	virtual Equipable * asEquipable();
};
