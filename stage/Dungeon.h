#pragma once

#include <list>
#include <vector>

class Entity;

#include "Stage.h"
#include "../CellType.h"
#include "../util/Position.h"
#include "../action/Action.h"
#include "SimpleLoader.h"

class Dungeon : public Stage, public std::enable_shared_from_this<Dungeon> {
public:
    Dungeon(const std::shared_ptr<State> &state, int id, int width, int height);

    void addEntity(std::shared_ptr<Entity> entity);
    void initializeEntity(std::shared_ptr<Entity> entity);

    std::list<std::shared_ptr<Entity>> getEntitiesAt(Position position);
    std::list<std::shared_ptr<Entity>> getEntitiesAt(Position position, const std::shared_ptr<Entity> &exclude);
    std::shared_ptr<Entity> getEntityAt(const Position position);

    CellType getCellType(const Position position);
    void setCellType(const Position position, const CellType type);

    void initializeCell(const Position position, const CellType type);

    const std::shared_ptr<State> & getState();

    int run(Game &game) override;
    void finish(int next);

    std::vector<Position> getTargetable(const Position position, Action::Range range, int minRange, int maxRange, bool move = false);
    std::vector<Position> getTargetedArea(const Position from, const Position to, const std::shared_ptr<Action> &action);
    std::vector<std::shared_ptr<Entity>> getTargeted(const std::shared_ptr<Entity> &from, Position to, const std::shared_ptr<Action> &action);

    const std::string &getName() const;

    void setName(const std::string &name);

    const int width;
    const int height;

    Position spawnPoint {0, 0};

private:
    std::string name;

    bool isRunning {false};
    int next {0};

    const int size;
    std::list<std::shared_ptr<Entity>> entities;
    std::vector<CellType> cells;
    std::vector<std::list<std::shared_ptr<Entity>>> cellEntities;

    std::vector<int> rangeTracker; // used for calculations in getTargetable and getTargeted


    std::list<std::shared_ptr<Entity>> &getCellListAt(Position position);

    bool onFeild(const Position position);

    bool onFeild(int x, int y);

    int &atRange(const Position position);

    int &atRange(int x, int y);

    void resetRange();

    friend class SimpleLoader;
};
