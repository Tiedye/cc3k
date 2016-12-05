#pragma once


#include <vector>
#include "../event/Listener.h"
#include "../entity/Entity.h"
#include "../util/Position.h"
#include "../CellType.h"

class DungeonRenderer : public Listener, public std::enable_shared_from_this<DungeonRenderer> {
public:
    DungeonRenderer();

    void changeCell(Position position); // neccessary to display grid updates

    void notify(EventInfo &info) final;
    const std::vector<EventType> listeningFor() const override;

    virtual void engage() = 0;
    virtual void disengage() = 0;

    void setDungeon(const std::shared_ptr<Dungeon> &dungeon);

protected:
    static const std::vector<EventType> eventTypes;

    virtual void entityMoved(const std::shared_ptr<Entity> &entity, const Position oldPos) = 0;
    virtual void entityAdded(const std::shared_ptr<Entity> &entity) = 0;
    virtual void entityRemoved(const std::shared_ptr<Entity> &entity) = 0;
    virtual void entityMissed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target) = 0;
    virtual void entityAttacked(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int damage) = 0;
    virtual void entityHealed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int heal) = 0;
    virtual void cellChanged(const Position position) = 0;

    std::shared_ptr<Dungeon> dungeon;
};


