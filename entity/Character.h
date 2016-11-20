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

	int getAttackStrength();
	int getSpellStrength();
	int getSpeed();
	int getTenacity();
	int getAccuracy();

protected:
	int attackStrength {0};
	int spellStrength {0};
	int speed {0};
	int tenacity {0};
	int accuracy {0};
};