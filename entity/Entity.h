#pragma once
#include "../EventTarget.h"
#include "../util/Size.h"
#include "../util/Position.h"
#include "../Listener.h"
#include "../FeatureSet.h"
#include "../util/Direction.h"
#include "../StatModifier.h"
#include "Stat.h"
#include "../util/EffectType.h"
#include <memory>
#include <list>
#include <map>
#include <set>

class Entity {
public:
	class Target : public EventTarget {
	public:
		Target( Entity *entity );
		Entity * asEntity() override;
	private:
		Entity *entity;
	};

    void doTurn();

	virtual ~Entity();

    // Entity is an EventEmmiter, as their are no other th functionality is included here
    virtual void addListener(Listener &listener);
    virtual void removeListener(Listener &listener);
	virtual std::unique_ptr<EventTarget> getAsTarget();

    virtual void addModifier(StatModifier &modifier);
    virtual void removeModifier(StatModifier &modifier);

    virtual void addAction(Action &action) = default;
    virtual void removeAction(Action &action) = default;

    void addFeatureSet(FeatureSet &featureSet);
    void removeFeatureSet(FeatureSet &featureSet);

    void addTemporaryFeatureSet(FeatureSet featureSet, EffectType effectType, int numTurns);

    void attack(Entity *source, int amount);
    void damage(int damage);
    void knock(Entity *source, int distance, Direction direction);
    void move(Position destination);
    virtual void interact(Character *source);

    void addListReference(std::list<Entity*> &list, std::list<Entity*>::iterator reference);
    void removeListReference(std::list<Entity*> &list);

	Position getPosition();
	Size getSize();
	int getMaxHealth();
	int getHealth();
	int getInitiative();
	int getDefenceStrength();
	int getKnockbackResist();
protected:
	Position position {0,0};
    int health;

	Stat size {MINISCULE};
	Stat maxHealth;
	Stat initiative;
	Stat defenseStrength;
	Stat knockbackResist;

    void checkDead();

private:
    std::map<std::list<Entity*>*, std::list<Entity*>::iterator> listReferences;

    std::set<Listener*> interactedListeners;
    std::set<Listener*> interactedDoneListeners;
    std::set<Listener*> attackedListeners;
    std::set<Listener*> attackedDoneListeners;
    std::set<Listener*> movedListeners;
    std::set<Listener*> movedDoneListeners;
    std::set<Listener*> occupiedListeners;
    std::set<Listener*> occupiedDoneListeners;
    std::set<Listener*> createdListeners;
    std::set<Listener*> createdDoneListeners;
    std::set<Listener*> destroyedListeners;
    std::set<Listener*> destroyedDoneListeners;
};