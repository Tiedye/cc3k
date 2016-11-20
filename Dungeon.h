#pragma once

#include <list>
#include <vector>
#include "entity/Entity.h"
#include "CellType.h"
#include "Stage.h"
#include "util/Position.h"

class Dungeon : public Stage {
public:
    Dungeon(State &state, int width, int height);

    std::list<Entity *> getEntitiesAt(Position position);
    CellType getCellType(Position position);
    void start() override;
private:
    const int width;
    const int height;
    std::list<Entity*> entities;
    std::vector<CellType> cells;
    std::vector<std::list<Entity*>> cellEntities;
};


