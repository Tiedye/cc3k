#pragma once


#include <vector>
#include "../Listener.h"
#include "../entity/Entity.h"
#include "../util/Position.h"
#include "../CellType.h"

class DungeonRenderer : public Listener {
public:
    void changeCell(Position position);

    void notify(EventInfo info) final;
    bool isListeningFor(EventType type);

protected:
    DungeonRenderer(const std::vector<CellType> &cells);

    virtual void entityMoved(const Entity *entity, Position oldPos) = 0;
    virtual void entityCreated(const Entity *entity) = 0;
    virtual void entityDestroyed(const Entity *entity) = 0;
    virtual void cellChanged(Position position) = 0;

    const std::vector<CellType> &cells;
};


