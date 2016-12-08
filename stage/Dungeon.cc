#include "Dungeon.h"

#include <queue>
#include <algorithm>

#include "../Game.h"
#include "../display/ConsoleDungeonIO.h"


#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

std::list<std::shared_ptr<Entity>> Dungeon::getEntitiesAt(Position position) {
    return cellEntities[position.x+position.y*width];
}

std::list<std::shared_ptr<Entity>> Dungeon::getEntitiesAt(Position position, const shared_ptr<Entity> &exclude) {
    auto entityList = cellEntities[position.x+position.y*width];
    auto found = find(entityList.begin(), entityList.end(), exclude);
    if (found != entityList.end()) entityList.erase(found);
    return entityList;
}

int Dungeon::run(Game &game) {
    isRunning = true;
    next = defaultNext;
    state->currentDungeon = shared_from_this();
    state->player->move(spawnPoint);
    initializeEntity(state->player);
    state->dungeonRenderer->setDungeon(shared_from_this());
    state->dungeonRenderer->engage();
    int turnId {0};
    while (isRunning){
        if (state->player->isDead()) break;
        int currentInit {0};
        for (auto entityIter = entities.begin(); entityIter != entities.end();) {
            auto entity = *entityIter;

            entity->startTracking();
            currentInit = entity->getInitiative();

            entity->doTurn(*this, turnId);

            // handle if iterator invalidated
            if (entity->iteratorInvalid()) {
                entityIter = find_if(entities.begin(), entities.end(),
                                     [turnId, currentInit](const shared_ptr<Entity> searching) {
                                         return searching->lastTurnId() != turnId &&
                                                searching->getInitiative() >= currentInit;
                                     });
            } else {
                ++entityIter;
            }

            if(!isRunning) break;
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        ++turnId;
    }
    state->dungeonRenderer->disengage();
    state->player->removeFromContainers();
    return next;
}

CellType Dungeon::getCellType(const Position position) {
    if (onFeild(position)) {
        return cells[position.x + position.y * width];
    } else {
        return EMPTY;
    }
}

void Dungeon::setCellType(const Position position, const CellType type) {
    if (onFeild(position)) {
        cells[position.x + position.y * width] = type;
        state->dungeonRenderer->changeCell(position);
    }
}

void Dungeon::initializeCell(const Position position, const CellType type) {
    if (onFeild(position)) {
        cells[position.x + position.y * width] = type;
    }
}

Dungeon::Dungeon(const shared_ptr<State> &state, int id, int width, int height) : Stage(state, id), width{width}, height{height}, size{width * height}, cells(size), cellEntities(size), rangeTracker(size, -1) {}

bool compareInitiative(const shared_ptr<Entity> &a, const shared_ptr<Entity> &b) {
    return a->getInitiative() > b->getInitiative();
}

void Dungeon::addEntity(shared_ptr<Entity> entity) {
    entity->addListener(shared_from_this());
    entity->addListener(state->dungeonRenderer);
    entity->trigger(ADDED_TO_FLOOR);
    auto pos = entities.insert(upper_bound(entities.begin(), entities.end(), entity, compareInitiative), entity);
    entity->addListReference(entities, pos);
    list<shared_ptr<Entity>> &cellList = getCellListAt(entity->getPosition());
    cellList.push_front(entity);
    entity->addListReference(cellList, cellList.begin());
    entity->onFloor = true;
    entity->trigger(ADDED_TO_FLOOR_DONE);
}

void Dungeon::initializeEntity(std::shared_ptr<Entity> entity) {
    entity->addListener(shared_from_this());
    entity->addListener(state->dungeonRenderer);
    entities.push_front(entity);
    entity->addListReference(entities, entities.begin());
    list<shared_ptr<Entity>> &cellList = getCellListAt(entity->getPosition());
    cellList.push_front(entity);
    entity->addListReference(cellList, cellList.begin());
    entity->onFloor = true;
}


std::list<std::shared_ptr<Entity>> &Dungeon::getCellListAt(Position position) {
    return cellEntities[position.x+position.y*width];
}

std::shared_ptr<Entity> Dungeon::getEntityAt(const Position position) {
    shared_ptr<Entity> largest;
    if(!onFeild(position)) return largest;
    for(auto entity:getEntitiesAt(position)) {
        if (largest) {
            if (entity->getSize() > largest->getSize()) largest = entity;
        } else {
            largest = entity;
        }
    }
    return largest;
}

std::vector<Position>
Dungeon::getTargetable(const Character &source, const std::shared_ptr<Action> &action) {
    vector<Position> targets;
    Action::Range range {action->range};
    int minRange {action->getMinRange(source)};
    int maxRange {action->getRange(source)};
    Position position {source.getPosition()};
    switch (range) {
        case Action::ANY:
            for(int x {-maxRange}; x <= maxRange; ++x) {
                for(int y {-maxRange}; y <= maxRange; ++y) {
                    int dist {x < y ? x < -y ? x : y : x < -y ? y : x}; // LOGIC!
                    if (dist < 0) dist = -dist;
                    if (dist >= minRange) {
                        Position newTarget {y+position.y, x+position.x};
                        if (onFeild(newTarget) && action->canTarget(newTarget, state)) targets.push_back(newTarget);
                    } else {
                        y += (minRange - 1) * 2; // LOGIC!
                    }
                }
            }
            break;
        case Action::LOS:
            // TODO ViewField implementation
            break;
        case Action::PATH: {
            resetRange();
            queue<Position> toFill;
            toFill.push(position);
            atRange(position) = 0;
            while (!toFill.empty()) {
                Position currentPosition = toFill.front();
                toFill.pop();
                if (atRange(currentPosition) >= minRange) {
                    if (onFeild(currentPosition) && action->canTarget(currentPosition, state)) targets.push_back(currentPosition);
                }
                if (atRange(currentPosition) == maxRange) {
                    continue;
                }
                Position positions[8]{
                        {currentPosition.y + 1, currentPosition.x + 1},
                        {currentPosition.y + 1, currentPosition.x},
                        {currentPosition.y + 1, currentPosition.x - 1},
                        {currentPosition.y,     currentPosition.x + 1},
                        {currentPosition.y,     currentPosition.x - 1},
                        {currentPosition.y - 1, currentPosition.x + 1},
                        {currentPosition.y - 1, currentPosition.x},
                        {currentPosition.y - 1, currentPosition.x - 1}
                };
                for (Position &pos:positions) {
                    if (onFeild(pos) && atRange(pos) == -1 && (getCellType(pos) != WALL && getCellType(pos) != EMPTY)) {
                        atRange(pos) = atRange(currentPosition) + 1;
                        toFill.push(pos);
                    }
                }
            }
            break;
        }
        case Action::STRICT_PATH: {
            resetRange();
            queue<Position> toFill;
            toFill.push(position);
            atRange(position) = 0;
            while (!toFill.empty()) {
                Position currentPosition = toFill.front();
                toFill.pop();
                if (atRange(currentPosition) >= minRange) {
                    if (onFeild(currentPosition) && action->canTarget(currentPosition, state)) targets.push_back(currentPosition);
                }
                if (atRange(currentPosition) == maxRange) {
                    continue;
                }
                Position positions[8]{
                        {currentPosition.y + 1, currentPosition.x + 1},
                        {currentPosition.y + 1, currentPosition.x},
                        {currentPosition.y + 1, currentPosition.x - 1},
                        {currentPosition.y,     currentPosition.x + 1},
                        {currentPosition.y,     currentPosition.x - 1},
                        {currentPosition.y - 1, currentPosition.x + 1},
                        {currentPosition.y - 1, currentPosition.x},
                        {currentPosition.y - 1, currentPosition.x - 1}
                };
                for (Position &pos:positions) {
                    auto entityAt = getEntityAt(pos);
                    if (onFeild(pos) && atRange(pos) == -1 && (getCellType(pos) != WALL && getCellType(pos) != EMPTY) &&
                        (!entityAt || entityAt->getSize() == MINISCULE)) {
                        atRange(pos) = atRange(currentPosition) + 1;
                        toFill.push(pos);
                    }
                }
            }
            break;
        }
    }

    return targets;
}


bool Dungeon::onFeild(const Position position) {
    return position.y >= 0 && position.y < height && position.x >= 0 && position.x < width;
}

bool Dungeon::onFeild(int x, int y) {
    return y >= 0 && y < height && x >= 0 && x < width;
}

int &Dungeon::atRange(const Position position) {
    return rangeTracker[position.x + position.y*width];
}

int &Dungeon::atRange(int x, int y) {
    return rangeTracker[x + y*width];
}

void Dungeon::resetRange() {
    fill(rangeTracker.begin(), rangeTracker.end(), -1);
}

const shared_ptr<State> Dungeon::getState() {
    return state;
}

std::vector<std::shared_ptr<Entity>> Dungeon::getTargeted(const shared_ptr<Entity> &from, Position to,
                                                          const shared_ptr<Action> &action) {
    vector<shared_ptr<Entity>> targeted;
    for (Position target:getTargetedArea(from->getPosition(), to, action)){
        if (action->aoe) {
            list<shared_ptr<Entity>> atTarget;
            if (action->targets & Action::SELF) {
                atTarget = getEntitiesAt(target);
            } else {
                atTarget = getEntitiesAt(target, from);
            }
            targeted.insert(targeted.end(), atTarget.begin(), atTarget.end());
        } else {
            auto atTarget = getEntityAt(target);
            if (atTarget && (atTarget != from || action->targets & Action::SELF)) {
                targeted.push_back(atTarget);
            }
        }
    }
    return targeted;
}

std::vector<Position> Dungeon::getTargetedArea(const Position from, const Position to, const std::shared_ptr<Action> &action) {
    vector<Position> targeted;
    switch (action->shape) {
        case Action::POINT: {
            targeted.push_back(to);
            break;
        }
        case Action::LINE: {
            Position delta = to - from;
            if (delta.x == 0) { // handle vertical case
                if (delta.y > 0) {
                    for (int y = from.y; y <= to.y; ++y) {
                        targeted.emplace_back(y, from.x);
                    }
                } else {
                    for (int y = from.y; y >= to.y; --y) {
                        targeted.emplace_back(y, from.x);
                    }
                }
            } else { // other cases TODO: test this
                float slope = delta.y;
                slope /= delta.x;
                float soFar{0.5};
                int xDir{delta.x > 0 ? 1 : -1};
                int yDir{delta.y > 0 ? 1 : -1};
                Position current{from};
                targeted.push_back(current);
                do {
                    if (soFar < 1 && soFar > 0) {
                        soFar += slope;
                        current.shiftX(xDir);
                    } else if (soFar < 2 && soFar > -1) {
                        soFar += slope - 1;
                        current.shiftX(xDir);
                        current.shiftY(yDir);
                    } else {
                        soFar -= 1;
                        current.shiftY(yDir);
                    }
                    targeted.push_back(current);
                } while (current != to);
            }
            break;
        }
        case Action::SQUARE: {
            int range{action->aoeSize};
            for (int x{-range}; x <= range; ++x) {
                for (int y{-range}; y <= range; ++y) {
                    targeted.emplace_back(y + to.y, x + to.x);
                }
            }
            break;
        }
    }
    return targeted;
}

std::vector<Position> Dungeon::path(const Position from, const Position to, const std::shared_ptr<Action> &action) {
    resetRange();
    queue<Position> toFill;
    toFill.push(from);
    atRange(from) = 0;
    while (!toFill.empty()) {
        Position currentPosition = toFill.front();
        toFill.pop();
        if (currentPosition == to) {
            break;
        }
        Position positions[8]{
                {currentPosition.y + 1, currentPosition.x + 1},
                {currentPosition.y + 1, currentPosition.x},
                {currentPosition.y + 1, currentPosition.x - 1},
                {currentPosition.y,     currentPosition.x + 1},
                {currentPosition.y,     currentPosition.x - 1},
                {currentPosition.y - 1, currentPosition.x + 1},
                {currentPosition.y - 1, currentPosition.x},
                {currentPosition.y - 1, currentPosition.x - 1}
        };
        for (Position &pos:positions) {
            auto entityAt = getEntityAt(pos);
            if (onFeild(pos) && atRange(pos) == -1 && (getCellType(pos) != WALL && getCellType(pos) != EMPTY) &&
                (!entityAt || entityAt->getSize() == MINISCULE)) {
                atRange(pos) = atRange(currentPosition) + 1;
                toFill.push(pos);
            }
        }
    }
    if (atRange(to)) {
        vector<Position> path(atRange(to)+1);
        Position currentPosition = to;
        for (int d = atRange(to); d >= 0; --d) {
            path[d] = to;
            Position positions[8]{
                    {currentPosition.y + 1, currentPosition.x + 1},
                    {currentPosition.y + 1, currentPosition.x},
                    {currentPosition.y + 1, currentPosition.x - 1},
                    {currentPosition.y,     currentPosition.x + 1},
                    {currentPosition.y,     currentPosition.x - 1},
                    {currentPosition.y - 1, currentPosition.x + 1},
                    {currentPosition.y - 1, currentPosition.x},
                    {currentPosition.y - 1, currentPosition.x - 1}
            };
            for (Position &pos:positions) {
                if (atRange(pos) == d-1) {
                    currentPosition = pos;
                    break;
                }
            }
        }
        return path;
    } else {
        return {};
    }
}

void Dungeon::finish(int next) {
    isRunning = false;
    Dungeon::next = next;
}

const string &Dungeon::getName() const {
    return name;
}

void Dungeon::setName(const string &name) {
    Dungeon::name = name;
}


void Dungeon::notify(EventInfo &info) {
    auto entity = info.primary->asEntity();
    auto &oldCellList = getCellListAt(info.eventPosition);
    auto result = find(oldCellList.begin(), oldCellList.end(), entity);
    if(result != oldCellList.end()) {
        oldCellList.erase(result);
        entity->removeListReference(oldCellList);
        auto &newCellList = getCellListAt(entity->getPosition());
        newCellList.push_front(entity);
        entity->addListReference(newCellList, newCellList.begin());
    }
}

const std::vector<EventType> Dungeon::listeningFor() const {
    return {MOVE_DONE};
}