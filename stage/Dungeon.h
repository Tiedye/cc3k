#pragma once

#include <list>
#include <vector>
#include "../entity/Entity.h"
#include "../CellType.h"
#include "Stage.h"
#include "../util/Position.h"

class Dungeon : public Stage {
public:
    Dungeon(State &state, int width, int height);

    void addEntity(Entity &entity);

    std::list<Entity *> getEntitiesAt(Position position);
    CellType getCellType(Position position);
    void start(Game &game) override;
private:
    const int width;
    const int height;
    std::list<Entity*> entities;
    std::vector<CellType> cells;
    std::vector<std::list<Entity*>> cellEntities;

    std::list<Entity*> & getCellListAt(Position position);
};


