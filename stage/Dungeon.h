#pragma once

#include <list>
#include <vector>
#include "../entity/Entity.h"
#include "../CellType.h"
#include "Stage.h"
#include "../util/Position.h"

class Dungeon : public Stage {
public:
    Dungeon(State &state, int id, int width, int height);

    void addEntity(Entity &entity);

    std::list<std::shared_ptr<Entity>> getEntitiesAt(Position position);
    CellType getCellType(Position position);
    void start(Game &game) override;
private:
    const int width;
    const int height;
    std::list<std::shared_ptr<Entity>> entities;
    std::vector<CellType> cells;
    std::vector<std::list<std::shared_ptr<Entity>>> cellEntities;

    std::list<std::shared_ptr<Entity>> & getCellListAt(Position position);
};


