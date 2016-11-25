#include "Dungeon.h"

using namespace std;

std::list<std::shared_ptr<Entity>> Dungeon::getEntitiesAt(Position position) {
    return cellEntities[position.col+position.row*width];
}

void Dungeon::start(Game &game) {
    return;
}

CellType Dungeon::getCellType(Position position) {
    if (position.row < height && position.row >= 0 && position.col < width && position.col >= 0) {
        return cells[position.col + position.row + width];
    } else {
        return WALL;
    }
}

Dungeon::Dungeon(State &state, int id, int width, int height) : Stage(state, id), width{width}, height{height} {}

void Dungeon::addEntity(Entity &entity) {
    entities.push_front(&entity);
    entity.addListReference(entities, entities.begin());
    list<shared_ptr<Entity>> &cellList = getCellListAt(entity.getPosition());
    cellList.push_front(&entity);
    entity.addListReference(cellList, cellList.begin());
}

std::list<std::shared_ptr<Entity>> &Dungeon::getCellListAt(Position position) {
    return cellEntities[position.col+position.row*width];
}

std::shared_ptr<Entity> Dungeon::getEntityAt(Position position) {
    shared_ptr<Entity> largest;
    for(auto entity:getEntitiesAt(position)) {
        if (largest) {
            if (entity->getSize() > largest->getSize()) largest = entity;
        } else {
            largest = entity;
        }
    }
    return largest;
}
