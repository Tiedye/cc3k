#pragma once

#include <memory>
#include <list>
#include <map>
#include <set>
#include <vector>

#include "../util/Size.h"
#include "../util/Position.h"
#include "../util/Direction.h"
#include "../util/EffectType.h"
#include "../event/EventType.h"

#include "Stat.h"

#include "../event/EventTarget.h"
#include "../event/EventInfo.h"

class Listener;
class FeatureSet;
class StatModifier;
class Action;
class Dungeon;

class Entity : public std::enable_shared_from_this<Entity> {
public:
	class Target : public EventTarget {
	public:
		Target(const std::shared_ptr<Entity> &entity);
		std::shared_ptr<Entity> asEntity() override;
	private:
        std::shared_ptr<Entity> entity;
	};

    Entity(std::string name = "");
    Entity(const Entity&other);

	virtual void doTurn(Dungeon &dungeon);

    void removeFromContainers();
	virtual ~Entity() = default;

    // Entity is an EventEmmiter, as their are no other th functionality is included here
    void addListener(const std::shared_ptr<Listener> &listener);
    void removeListener(const std::shared_ptr<Listener> &listener);
	virtual std::unique_ptr<EventTarget> getAsTarget();

    void addModifier(const StatModifier &modifier, int modNumerator = 1, int modDenominator = 1);
    void removeModifier(const StatModifier &modifier, int modNumerator = 1, int modDenominator = 1);

    void addAction(const std::shared_ptr<Action> &action);
    void removeAction(const std::shared_ptr<Action> &action);

    void addFeatureSet(const FeatureSet &featureSet, int modNumerator = 1, int modDenominator = 1);
    void removeFeatureSet(const FeatureSet &featureSet, int modNumerator = 1, int modDenominator = 1);

    void addTemporaryFeatureSet(const std::shared_ptr<Entity> &source, const std::shared_ptr<FeatureSet> &featureSet,
                                EffectType effectType,
                                int numTurns);

    bool isA(int type);

    // acting upon this entity
	void create();

    void occupy(const std::shared_ptr<Entity> &source);

    void interact(const std::shared_ptr<Entity> &source);

    void damage(const int amount, const std::shared_ptr<Entity> &source);
    int damage(const int damage);
    void heal(const int amount, const std::shared_ptr<Entity> &source);
    void heal(const int amount);
    void move(const int distance, const Direction direction, const std::shared_ptr<Entity> &source);
    void move(const int distance, const Direction direction);
    void move(const Position destination, const std::shared_ptr<Entity> &source);
    void move(const Position destination);
    void kill(const std::shared_ptr<Entity> &source);
    void destroy();

	virtual void give(const std::shared_ptr<Item> &item);
    virtual void equip(const std::shared_ptr<Character> &onto);
    virtual void unequip();
    virtual void consume(const std::shared_ptr<Character> &by);

    int getHealth();
    void setHealth(const int amount);
    bool isDead();

	void addListReference(std::list<std::shared_ptr<Entity>> &list, std::list<std::shared_ptr<Entity>>::iterator reference);
	void addListReference(std::list<std::shared_ptr<Item>> &list, std::list<std::shared_ptr<Item>>::iterator reference);
	void removeListReference(std::list<std::shared_ptr<Entity>> &list);
	void removeListReference(std::list<std::shared_ptr<Item>> &list);

	Position getPosition();
	int getSize();
	int getMaxHealth();
	int getInitiative();
	int getDefenceStrength();
    int getKnockbackResist();
    int getDodge();

    const std::string & getName();

    bool onFloor {false};

    char representation;

    virtual std::shared_ptr<Entity> clone();

	void trigger(EventType eventType);
	void trigger(EventType eventType, const std::shared_ptr<Entity> &secondary);
	void trigger(EventType eventType, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, Position position);
	void trigger(EventType eventType, Position position, const std::shared_ptr<Entity> &secondary);
	void trigger(EventType eventType, Position position, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, int integer);
	void trigger(EventType eventType, int integer, const std::shared_ptr<Entity> &secondary);
	void trigger(EventType eventType, int integer, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, float num);
	void trigger(EventType eventType, float num, const std::shared_ptr<Entity> &secondary);
	void trigger(EventType eventType, float num, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, double num);
	void trigger(EventType eventType, double num, const std::shared_ptr<Entity> &secondary);
	void trigger(EventType eventType, double num, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(EventType eventType, EventInfo::Data &reference);
	void trigger(EventType eventType, EventInfo::Data &reference, const std::shared_ptr<Entity> &secondary);
	void trigger(EventType eventType, EventInfo::Data &reference, const std::vector<std::shared_ptr<Entity>> &secondaries);

protected:
    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    std::string name;

    virtual Stat & getCorrespondingStat(const StatModifier &modifier);

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

    bool dead {false};

    friend class Loader;
};