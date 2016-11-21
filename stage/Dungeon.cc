#include "Dungeon.h"

using namespace std;

std::list<Entity *> Dungeon::getEntitiesAt(Position position) {
    return cellEntities[position.col+position.row*width];
}

void Dungeon::start(Game &game) {
    return;
}

CellType Dungeon::getCellType(Position position) {
    return cells[position.col+position.row+width];
}

Dungeon::Dungeon(State &state, int width, int height) : Stage(state, 0), width{width}, height{height} {}

void Dungeon::addEntity(Entity &entity) {
    entities.push_front(&entity);
    entity.addListReference(entities, entities.begin());
    list<Entity *> &cellList = getCellListAt(entity.getPosition());
    cellList.push_front(&entity);
    entity.addListReference(cellList, cellList.begin());
}

std::list<Entity *> &Dungeon::getCellListAt(Position position) {
    return cellEntities[position.col+position.row*width];
}
