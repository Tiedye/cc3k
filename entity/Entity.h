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
#include "../controller/HasAIData.h"

class Listener;
class FeatureSet;
class StatModifier;
class Action;
class Dungeon;

class Entity : public std::enable_shared_from_this<Entity>, public HasAIData {
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

	virtual void doTurn(Dungeon &dungeon, const int turnId);
    int lastTurnId() const;

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
	void makeA(int type);

    // acting upon this entity
	void create();

    void occupy(const std::shared_ptr<Entity> &source);

    void interact(const std::shared_ptr<Entity> &source);

    int damage(const int amount, const std::shared_ptr<Entity> &source);
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
    virtual bool consumable();
    virtual bool equippable();
    virtual int equippedSlot();

    int getHealth();
    void setHealth(const int amount);
    bool isDead();

	void startTracking();
	void addListReference(std::list<std::shared_ptr<Entity>> &list, std::list<std::shared_ptr<Entity>>::iterator reference);
	void addListReference(std::list<std::shared_ptr<Item>> &list, std::list<std::shared_ptr<Item>>::iterator reference);
	void removeListReference(std::list<std::shared_ptr<Entity>> &list);
	void removeListReference(std::list<std::shared_ptr<Item>> &list);
	bool iteratorInvalid();

	Position getPosition() const;
	int getSize() const;
	int getMaxHealth() const;
	int getInitiative() const;
	int getDefenceStrength() const;
    int getKnockbackResist() const;
    int getTenacity() const;
    int getDodge() const;

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

	template <typename Derived>
	std::shared_ptr<Derived> shared_from_base()
	{
		return std::static_pointer_cast<Derived>(shared_from_this());
	}

protected:

    std::string name;

    virtual Stat & getCorrespondingStat(const StatModifier &modifier);

	Position position {0,0};
    int health = {0};

	Stat size; // base value 0
	Stat maxHealth {1}; // base value 1
	Stat initiative; // base value 0
	Stat defenseStrength; // base value 0
	Stat knockbackResist; // base value 0
    Stat tenacity; // base value 0
    Stat dodge; // base value 0
    //Stat tempModNumerator {1};
    //Stat tempModDenominator {1};

    void checkDead();

    std::set<std::shared_ptr<Action>> actions;

private:
//    bool iteratorsLocked {false};
//    struct ListReferenceOp {
//        enum Op {
//            ADD,
//            REMOVE,
//            CLEAR
//        };
//        Op op;
//        std::list<std::shared_ptr<Entity>> *list;
//        std::list<std::shared_ptr<Entity>>::iterator reference;
//        ListReferenceOp(const Op op, std::list<std::shared_ptr<Entity>> *list = nullptr, const std::list<std::shared_ptr<Entity>>::iterator reference = {});
//    };
//    std::vector<ListReferenceOp> queuedOps;

	bool trackIteratorValidity {false};
    int lastTurn {-1};

    int turnCount {0};

    void checkTempFeatures();
    struct TempFeatureSet {
        std::shared_ptr<FeatureSet> set;
        std::shared_ptr<Entity> source;
        EffectType effectType;
        int modNumerator;
        int modDenominator;
    };

    struct ListenerSort {
        bool operator()(const std::shared_ptr<Listener> &a, const std::shared_ptr<Listener> &b) const;
    };

    std::map<std::list<std::shared_ptr<Entity>>*, std::list<std::shared_ptr<Entity>>::iterator> listReferences;

	std::map<EventType, std::set<std::shared_ptr<Listener>, ListenerSort>> listeners;

    std::map<int, TempFeatureSet> tempFeatureSets;

    std::set<int> types;

    bool dead {false};

    friend class Loader;
};