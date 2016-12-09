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
		std::shared_ptr<Entity> as_entity() override;
	private:
        std::shared_ptr<Entity> entity;
	};

    Entity(std::string name = "");
    Entity(const Entity&other);

	virtual void do_turn(Dungeon &dungeon, const int turn_id);
    int last_turn_id() const;

    void remove_from_containers();
	virtual ~Entity() = default;

    // Entity is an EventEmmiter, as their are no other th functionality is included here
    void add_listener(const std::shared_ptr<Listener> &listener);
    void remove_listener(const std::shared_ptr<Listener> &listener);
	virtual std::unique_ptr<EventTarget> get_as_target();

    void add_modifier(const StatModifier &modifier, int mod_numerator = 1, int mod_denominator = 1);
    void remove_modifier(const StatModifier &modifier, int mod_numerator = 1, int mod_denominator = 1);

    void add_action(const std::shared_ptr<Action> &action);
    void remove_action(const std::shared_ptr<Action> &action);

    void add_feature_set(const FeatureSet &feature_set, int mod_numerator = 1, int mod_denominator = 1);
    void remove_feature_set(const FeatureSet &feature_set, int mod_numerator = 1, int mod_denominator = 1);

    void add_temporary_feature_set(const std::shared_ptr<Entity> &source, const std::shared_ptr<FeatureSet> &feature_set,
                                EffectType effect_type,
                                int num_turns);
    void clear_temporary_feature_sets();

    bool is_a(int type);
	void make_a(int type);

    // acting upon this entity
	void create();

    void occupy(const std::shared_ptr<Entity> &source);

    void interact(const std::shared_ptr<Entity> &source);

    int damage(const int amount, const std::shared_ptr<Entity> &source);
    int damage(const int damage);
    int heal(const int amount, const std::shared_ptr<Entity> &source);
    int heal(const int amount);
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
    virtual int equipped_slot();

    int get_health();
    void set_health(const int amount);
    bool is_dead();

	void add_score(const int value);
	int current_score() const;
    void set_score(const int value);

	void start_tracking();
	void add_list_reference(std::list<std::shared_ptr<Entity>> &list, std::list<std::shared_ptr<Entity>>::iterator reference);
	void add_list_reference(std::list<std::shared_ptr<Item>> &list, std::list<std::shared_ptr<Item>>::iterator reference);
	void remove_list_reference(std::list<std::shared_ptr<Entity>> &list);
	void remove_list_reference(std::list<std::shared_ptr<Item>> &list);
	bool iterator_invalid();

	Position get_position() const;
	int get_size() const;
	int get_max_health() const;
	int get_initiative() const;
	int get_defence_strength() const;
    int get_knockback_resist() const;
    int get_tenacity() const;
    int get_dodge() const;

    const std::string & get_name();

    bool on_floor {false};

    char representation;

    virtual std::shared_ptr<Entity> clone();

	void trigger(const EventType event_type);
	void trigger(const EventType event_type, const std::shared_ptr<Entity> &secondary);
	void trigger(const EventType event_type, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(const EventType event_type, const Position position);
	void trigger(const EventType event_type, const Position position, const std::shared_ptr<Entity> &secondary);
	void trigger(const EventType event_type, const Position position, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(const EventType event_type, const int integer);
	void trigger(const EventType event_type, const int integer, const std::shared_ptr<Entity> &secondary);
	void trigger(const EventType event_type, const int integer, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(const EventType event_type, const float num);
	void trigger(const EventType event_type, const float num, const std::shared_ptr<Entity> &secondary);
	void trigger(const EventType event_type, const float num, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(const EventType event_type, const double num);
	void trigger(const EventType event_type, const double num, const std::shared_ptr<Entity> &secondary);
	void trigger(const EventType event_type, const double num, const std::vector<std::shared_ptr<Entity>> &secondaries);
	void trigger(const EventType event_type, EventInfo::Data &reference);
	void trigger(const EventType event_type, EventInfo::Data &reference, const std::shared_ptr<Entity> &secondary);
	void trigger(const EventType event_type, EventInfo::Data &reference, const std::vector<std::shared_ptr<Entity>> &secondaries);

	template <typename Derived>
	std::shared_ptr<Derived> shared_from_base()
	{
		return std::static_pointer_cast<Derived>(shared_from_this());
	}

protected:

    std::string name;

    virtual Stat & get_corresponding_stat(const StatModifier &modifier);

	Position position {0,0};
    int health = {0};

	Stat size; // base value 0
	Stat max_health {1}; // base value 1
	Stat initiative; // base value 0
	Stat defense_strength; // base value 0
	Stat knockback_resist; // base value 0
    Stat tenacity; // base value 0
    Stat dodge; // base value 0
    //Stat temp_mod_numerator {1};
    //Stat temp_mod_denominator {1};

    void check_dead();

    std::set<std::shared_ptr<Action>> actions;

private:
//    bool iterators_locked {false};
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
//    std::vector<ListReferenceOp> queued_ops;

	bool track_iterator_validity {false};
    int last_turn {-1};

    int turn_count {0};

    void check_temp_features();
    struct TempFeatureSet {
        std::shared_ptr<FeatureSet> set;
        std::shared_ptr<Entity> source;
        EffectType effect_type;
        int mod_numerator;
        int mod_denominator;
    };

    struct ListenerSort {
        bool operator()(const std::shared_ptr<Listener> &a, const std::shared_ptr<Listener> &b) const;
    };

    std::map<std::list<std::shared_ptr<Entity>>*, std::list<std::shared_ptr<Entity>>::iterator> list_references;

	std::map<EventType, std::set<std::shared_ptr<Listener>, ListenerSort>> listeners;

    std::map<int, TempFeatureSet> temp_feature_sets;

    std::set<int> types;

    bool dead {false};

    int score {0};

    friend class Loader;
};