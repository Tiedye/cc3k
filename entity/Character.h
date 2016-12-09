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
		std::shared_ptr<Entity> as_entity() override;
		std::shared_ptr<Character> as_character() override;
	private:
		std::shared_ptr<Character> character;
	};

    Character(std::string name);

    Character();

	Character(const Character&other);

    void do_turn(Dungeon &dungeon, const int turn_id) override;

    std::unique_ptr<EventTarget> get_as_target() override;

    void give(const std::shared_ptr<Item> &item) override;
	void equip(const std::shared_ptr<Equippable> &equippable);
	void unequip(const std::shared_ptr<Equippable> &equippable);
    void add_gold(const int value);
    bool remove_gold(const int value);
    int current_gold() const;

	const std::list<std::shared_ptr<Item>> &get_inventory() const;
    void clear_inventory();

	int get_attack_strength() const;
	int get_spell_strength() const;
	int get_speed() const;
	int get_accuracy() const;

    virtual std::shared_ptr<Entity> clone() override;

    std::shared_ptr<Controller> controller;

protected:
    Stat & get_corresponding_stat(const StatModifier &modifier) override;

    Stat attack_strength; // base value 0
    Stat spell_strength; // base value 0
    Stat speed; // base value 0
    Stat accuracy; // base value 0

    std::list<std::shared_ptr<Item>> inventory;
    std::map<int, std::shared_ptr<Equippable>> slots;

    int gold {0};

private:

	friend class Loader;
};