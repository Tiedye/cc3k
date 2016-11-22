#pragma once
#include "Entity.h"
#include <list>
#include <map>

class Character: public Entity {
public:
	class Target : public EventTarget {
	public:
		Target( std::shared_ptr<Character> &character );
		std::shared_ptr<Entity> asEntity() override;
		std::shared_ptr<Character> asCharacter() override;
	private:
		std::shared_ptr<Character> character;
	};

    void doTurn() override;

    std::unique_ptr<EventTarget> getAsTarget() override;

    void addAction(Action &action) override;
    void removeAction(Action &action) override;

    void give(std::shared_ptr<Item> item);

	int getAttackStrength();
	int getSpellStrength();
	int getSpeed();
	int getTenacity();
	int getAccuracy();

    virtual std::shared_ptr<Entity> clone() override;

protected:
    Stat & getCorrespondingStat(StatModifier &modifier) override;

    Stat attackStrength; // base value 0
    Stat spellStrength; // base value 0
    Stat speed; // base value 0
    Stat tenacity; // base value 0
    Stat accuracy; // base value 0

    std::list<std::shared_ptr<Item>> inventory;
    std::map<int, std::shared_ptr<Item>> slots;
};