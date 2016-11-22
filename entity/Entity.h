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

class Controller;

class Entity {
public:
	class Target : public EventTarget {
	public:
		Target( Entity *entity );
		Entity * asEntity() override;
	private:
		Entity *entity;
	};

    virtual void doTurn();

    void removeFromContainers();
	virtual ~Entity();

    // Entity is an EventEmmiter, as their are no other th functionality is included here
    void addListener(Listener &listener);
    void removeListener(Listener &listener);
	virtual std::unique_ptr<EventTarget> getAsTarget();

    void addModifier(StatModifier &modifier);
    void removeModifier(StatModifier &modifier);

    virtual void addAction(Action &action);
    virtual void removeAction(Action &action);

    void addFeatureSet(FeatureSet &featureSet);
    void removeFeatureSet(FeatureSet &featureSet);

    void addTemporaryFeatureSet(FeatureSet featureSet, EffectType effectType, int numTurns);

    bool isA(int type);

    // ating upon this entity
    void attack(Entity *source, int amount);
    void damage(int damage);
    void knock(Entity *source, int distance, Direction direction);
    void move(Position destination);
    void kill(Entity *source);
    void destroy();
    virtual void interact(Character *source);

    void addListReference(std::list<Entity *> &list, std::list<Entity *>::iterator reference);
    void removeListReference(std::list<Entity*> &list);

	Position getPosition();
	int getSize();
	int getMaxHealth();
	int getHealth();
	int getInitiative();
	int getDefenceStrength();
    int getKnockbackResist();
    int getDodge();

    virtual Entity * clone();
protected:
    virtual Stat * getCorrespondingStat(StatModifier &modifier);

	Position position {0,0};
    int health = {0};

	Stat size;
	Stat maxHealth;
	Stat initiative;
	Stat defenseStrength;
	Stat knockbackResist;
    Stat dodge;

    void checkDead();

    void trigger(EventType eventType);
    void trigger(EventType eventType, Entity* secondary);
    void trigger(EventType eventType, std::vector<Entity*> secondaries);
    void trigger(EventType eventType, Position position);
    void trigger(EventType eventType, Position position, Entity* secondary);
    void trigger(EventType eventType, Position position, std::vector<Entity*> secondaries);
    void trigger(EventType eventType, int integer);
    void trigger(EventType eventType, int integer, Entity* secondary);
    void trigger(EventType eventType, int integer, std::vector<Entity*> secondaries);
    void trigger(EventType eventType, float num);
    void trigger(EventType eventType, float num, Entity* secondary);
    void trigger(EventType eventType, float num, std::vector<Entity*> secondaries);
    void trigger(EventType eventType, double num);
    void trigger(EventType eventType, double num, Entity* secondary);
    void trigger(EventType eventType, double num, std::vector<Entity*> secondaries);

private:
    std::map<std::list<Entity*>*, std::list<Entity*>::iterator> listReferences;

	std::map<EventType, std::set<Listener*>> listeners;

    std::set<int> types;

	Controller *controller {nullptr};

    friend class Loader;
};