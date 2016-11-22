#pragma once
#include "Entity.h"
#include <list>
#include <map>

class Character: public Entity {
public:
	class Target : public EventTarget {
	public:
		Target( Character *character );
		Entity * asEntity() override;
		Character * asCharacter() override;
	private:
		Character *character;
	};

    void doTurn() override;

    std::unique_ptr<EventTarget> getAsTarget() override;

    void addAction(Action &action) override;
    void removeAction(Action &action) override;

    void give(Item *item);

	int getAttackStrength();
	int getSpellStrength();
	int getSpeed();
	int getTenacity();
	int getAccuracy();

    virtual Entity *clone() override;

protected:
    Stat *getCorrespondingStat(StatModifier &modifier) override;

    Stat attackStrength;
    Stat spellStrength;
    Stat speed;
    Stat tenacity;
    Stat accuracy;

    std::list<Item *> inventory;
    std::map<int, Item*> slots;
};