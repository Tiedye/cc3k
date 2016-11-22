#pragma once


#include <vector>
#include "../Listener.h"
#include "../entity/Entity.h"
#include "../util/Position.h"
#include "../CellType.h"

class DungeonRenderer : public Listener {
public:
    void changeCell(Position position);

    void notify(EventInfo &info) final;
    const std::vector<EventType> listeningFor() const override;

protected:
    static std::vector<EventType> eventTypes;

    DungeonRenderer(const std::vector<CellType> &cells);

    virtual void entityMoved(const std::shared_ptr<Entity> entity, Position oldPos) = 0;
    virtual void entityCreated(const std::shared_ptr<Entity> entity) = 0;
    virtual void entityDestroyed(const std::shared_ptr<Entity> entity) = 0;
    virtual void cellChanged(Position position) = 0;

    const std::vector<CellType> &cells;
};


