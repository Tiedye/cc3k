#pragma once
#include "Entity.h"

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

	std::unique_ptr<EventTarget> getAsTarget() override;

	virtual int getAttackStrength();
	virtual int getSpellStrength();
	virtual int getSpeed();
	virtual int getTenacity();

protected:
	int attackStrength {0};
	int spellStrenght {0};
	int speed {0};
	int tenacity {0};
};