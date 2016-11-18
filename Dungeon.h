#ifndef CC3K_DUNGEON_H
#define CC3K_DUNGEON_H

#include <list>
#include <vector>
#include "Entity.h"
#include "CellType.h"

class Dungeon {
public:
    std::list<Entity*> getEntitiesAt(int x, int y);
    CellType getCellType(int x, int y);
    void start();
private:
    const int width;
    const int height;
    std::list<Entity*> entities;
    std::vector<CellType> cells;
    std::vector<std::list<Entity*>> cellEntities;
};


#endif //CC3K_DUNGEON_H
