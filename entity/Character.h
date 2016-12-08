#pragma once

#include <list>
#include <map>

#include "Entity.h"
class Controller;

class Character: public Entity {
public:
	class Target : public EventTarget {
	public:
		Target(const std::shared_ptr<Character> &character);
		std::shared_ptr<Entity> asEntity() override;
		std::shared_ptr<Character> asCharacter() override;
	private:
		std::shared_ptr<Character> character;
	};

    Character(std::string name);

    Character();

	Character(const Character&other);

    void doTurn(Dungeon &dungeon, const int turnId) override;

    std::unique_ptr<EventTarget> getAsTarget() override;

    void give(const std::shared_ptr<Item> &item) override;
	void equip(const std::shared_ptr<Equippable> &equippable);
	void unequip(const std::shared_ptr<Equippable> &equippable);
    void addGold(const int value);
    bool removeGold(const int value);
    int currentGold() const;

	const std::list<std::shared_ptr<Item>> &getInventory() const;
    void clearInventory();

	int getAttackStrength() const;
	int getSpellStrength() const;
	int getSpeed() const;
	int getAccuracy() const;

    virtual std::shared_ptr<Entity> clone() override;

    std::shared_ptr<Controller> controller;

protected:
    Stat & getCorrespondingStat(const StatModifier &modifier) override;

    Stat attackStrength; // base value 0
    Stat spellStrength; // base value 0
    Stat speed; // base value 0
    Stat accuracy; // base value 0

    std::list<std::shared_ptr<Item>> inventory;
    std::map<int, std::shared_ptr<Equippable>> slots;

    int gold {0};

private:

	friend class Loader;
};