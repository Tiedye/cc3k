#pragma once
#include "EventTarget.h"
#include "Size.h"
#include <memory>

class Entity {
public:
	class Target : public EventTarget {
	public:
		Target( Entity *entity );
		Entity * asEntity() override;
	private:
		Entity *entity;
	};

	virtual ~Entity() = default;

	void addFeature();

	virtual std::unique_ptr<EventTarget> getAsTarget();

	virtual Size getSize();
	virtual int getMaxHealth();
	virtual int getHealth();
	virtual int getInitiative();
	virtual int getDefenceStrength();
	virtual int getKnockBackResist();
protected:
	Size size {MINISCULE};
	int maxHealth {0};
	int health {0};
	int initiative {0};
	int defenseStrength {0};
	int knockBackResist {0};
private:
	
};