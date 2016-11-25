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
#include "../State.h"
#include "../Game.h"
#include "../stage/Dungeon.h"
#include <memory>
#include <list>
#include <map>
#include <set>

class Controller;

class Entity : std::enable_shared_from_this<Entity> {
public:
	class Target : public EventTarget {
	public:
		Target( std::shared_ptr<Entity> &entity );
		std::shared_ptr<Entity> asEntity() override;
	private:
        std::shared_ptr<Entity> entity;
	};

	virtual void doTurn(Dungeon &dungeon);

    void removeFromContainers();
	virtual ~Entity() = default;

    // Entity is an EventEmmiter, as their are no other th functionality is included here
    void addListener(std::shared_ptr<Listener> listener);
    void removeListener(std::shared_ptr<Listener> listener);
	virtual std::unique_ptr<EventTarget> getAsTarget();

    void addModifier(const StatModifier &modifier, int modNumerator = 1, int modDenominator = 1);
    void removeModifier(const StatModifier &modifier, int modNumerator = 1, int modDenominator = 1);

    void addAction(std::shared_ptr<Action> action);
    void removeAction(std::shared_ptr<Action> action);

    void addFeatureSet(const FeatureSet &featureSet, int modNumerator = 1, int modDenominator = 1);
    void removeFeatureSet(const FeatureSet &featureSet, int modNumerator = 1, int modDenominator = 1);

    void addTemporaryFeatureSet(std::shared_ptr<Entity> source, const std::shared_ptr<FeatureSet> featureSet,
                                EffectType effectType,
                                int numTurns);

    bool isA(int type);

    // acting upon this entity
	void create();

    void occupy(std::shared_ptr<Entity> &source);

    void interact(std::shared_ptr<Entity> source);

    void damage(std::shared_ptr<Entity> source, int amount);
    int damage(int damage);
    void heal(std::shared_ptr<Entity> source, int amount);
    void heal(int amount);
    void move(std::shared_ptr<Entity> &source, int distance, Direction direction);
    void move(int distance, Direction direction);
    void move(std::shared_ptr<Entity> &source, Position destination);
    void move(Position destination);
    void kill(std::shared_ptr<Entity> &source);
    void destroy();

	virtual void give(std::shared_ptr<Item> item);
    virtual void equip(std::shared_ptr<Character> onto);
    virtual void unequip();
    virtual void consume(std::shared_ptr<Character> by);

    int getHealth();
    void setHealth(int amount);

    void addListReference(std::list<std::shared_ptr<Entity>> &list, std::list<std::shared_ptr<Entity>>::iterator reference);
    void removeListReference(std::list<std::shared_ptr<Entity>> &list);

	Position getPosition();
	int getSize();
	int getMaxHealth();
	int getInitiative();
	int getDefenceStrength();
    int getKnockbackResist();
    int getDodge();

    virtual std::shared_ptr<Entity> clone();

	void trigger(EventType eventType);
	void trigger(EventType eventType, std::shared_ptr<Entity> secondary);
	void trigger(EventType eventType, std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, Position position);
	void trigger(EventType eventType, Position position, std::shared_ptr<Entity> secondary);
	void trigger(EventType eventType, Position position, std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, int integer);
	void trigger(EventType eventType, int integer, std::shared_ptr<Entity> secondary);
	void trigger(EventType eventType, int integer, std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, float num);
	void trigger(EventType eventType, float num, std::shared_ptr<Entity> secondary);
	void trigger(EventType eventType, float num, std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, double num);
	void trigger(EventType eventType, double num, std::shared_ptr<Entity> secondary);
	void trigger(EventType eventType, double num, std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, EventInfo::Data &reference);
	void trigger(EventType eventType, EventInfo::Data &reference, std::shared_ptr<Entity> secondary);
	void trigger(EventType eventType, EventInfo::Data &reference, std::vector<std::shared_ptr<Entity>> &secondaries);

protected:
    virtual Stat & getCorrespondingStat(StatModifier &modifier);

	Position position {0,0};
    int health = {0};

	Stat size; // base value 0
	Stat maxHealth {1}; // base value 1
	Stat initiative; // base value 0
	Stat defenseStrength; // base value 0
	Stat knockbackResist; // base value 0
    Stat dodge; // base value 0
    //Stat tempModNumerator {1};
    //Stat tempModDenominator {1};

    void checkDead();

    std::set<std::shared_ptr<Action>> actions;

private:
    int turnCount {0};

    void checkTempFeatures();
    struct TempFeatureSet {
        std::shared_ptr<FeatureSet> set;
        std::shared_ptr<Entity> source;
        EffectType effectType;
        int modNumerator;
        int modDenominator;
    };

    std::map<std::list<std::shared_ptr<Entity>>*, std::list<std::shared_ptr<Entity>>::iterator> listReferences;

	std::map<EventType, std::set<std::shared_ptr<Listener>>> listeners;

    std::map<int, TempFeatureSet> tempFeatureSets;

    std::set<int> types;

    friend class Loader;
};