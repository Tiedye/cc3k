#include "Dungeon.h"

std::list<Entity *> Dungeon::getEntitiesAt(Position position) {
    return cellEntities[position.col+position.row*width];
}

void Dungeon::start() {
    return;
}

CellType Dungeon::getCellType(Position position) {
    return cells[position.col+position.row+width];
}

Dungeon::Dungeon(State &state, int width, int height) : Stage(state), width{width}, height{height} {}
