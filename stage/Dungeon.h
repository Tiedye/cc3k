#pragma once

#include <list>
#include <vector>

class Entity;

#include "Stage.h"
#include "../CellType.h"
#include "../util/Position.h"
#include "../action/Action.h"
#include "SimpleLoader.h"

class Dungeon : public Stage, public Listener, public std::enable_shared_from_this<Dungeon> {
public:
    Dungeon(const std::shared_ptr<State> &state, int id, int width, int height);

    void add_entity(std::shared_ptr<Entity> entity);
    void initialize_entity(std::shared_ptr<Entity> entity);

    std::list<std::shared_ptr<Entity>> get_entities_at(Position position);
    std::list<std::shared_ptr<Entity>> get_entities_at(Position position, const std::shared_ptr<Entity> &exclude);
    std::shared_ptr<Entity> get_entity_at(const Position position);

    CellType get_cell_type(const Position position);
    void set_cell_type(const Position position, const CellType type);

    void initialize_cell(const Position position, const CellType type);

    const std::shared_ptr<State> get_state();

    int run(Game &game) override;
    void finish(int next);

    std::vector<Position> path(const Position from, const Position to, const std::shared_ptr<Action> &action);

    std::vector<Position> get_targetable(const Character &source, const std::shared_ptr<Action> &action);
    std::vector<Position> get_targeted_area(const Position from, const Position to, const std::shared_ptr<Action> &action);
    std::vector<std::shared_ptr<Entity>> get_targeted(const std::shared_ptr<Entity> &from, Position to, const std::shared_ptr<Action> &action);

    const std::string &get_name() const;

    void set_name(const std::string &name);

    const int width;
    const int height;

    Position spawn_point {0, 0};

    void notify(EventInfo &info) override;

    const std::vector<EventType> listening_for() const override;

    int default_next {-1};

private:
    std::string name;

    bool is_running {false};
    int next {0};

    const int size;
    std::list<std::shared_ptr<Entity>> entities;
    std::vector<CellType> cells;
    std::vector<std::list<std::shared_ptr<Entity>>> cell_entities;

    std::vector<int> range_tracker; // used for calculations in get_targetable and get_targeted


    std::list<std::shared_ptr<Entity>> &get_cell_list_at(Position position);

    bool on_feild(const Position position);

    bool on_feild(int x, int y);

    int &at_range(const Position position);

    int &at_range(int x, int y);

    void reset_range();

    friend class SimpleLoader;
};
