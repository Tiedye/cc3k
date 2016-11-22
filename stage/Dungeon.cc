#include "Dungeon.h"

using namespace std;

std::list<std::shared_ptr<Entity>> Dungeon::getEntitiesAt(Position position) {
    return cellEntities[position.col+position.row*width];
}

void Dungeon::start(Game &game) {
    return;
}

CellType Dungeon::getCellType(Position position) {
    return cells[position.col+position.row+width];
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
