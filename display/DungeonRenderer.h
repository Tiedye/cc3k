#pragma once


#include <vector>
#include "../event/Listener.h"
#include "../entity/Entity.h"
#include "../util/Position.h"
#include "../CellType.h"

class DungeonRenderer : public Listener, public std::enable_shared_from_this<DungeonRenderer> {
public:
    DungeonRenderer();

    void change_cell(Position position); // neccessary to display grid updates

    void notify(EventInfo &info) final;
    const std::vector<EventType> listening_for() const override;

    virtual void engage() = 0;
    virtual void disengage() = 0;

    void set_dungeon(const std::shared_ptr<Dungeon> &dungeon);

protected:
    static const std::vector<EventType> event_types;

    virtual void entity_moved(const std::shared_ptr<Entity> &entity, const Position old_pos) = 0;
    virtual void entity_added(const std::shared_ptr<Entity> &entity) = 0;
    virtual void entity_removed(const std::shared_ptr<Entity> &entity) = 0;
    virtual void entity_missed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target) = 0;
    virtual void entity_attacked(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int damage) = 0;
    virtual void entity_healed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int heal) = 0;
    virtual void cell_changed(const Position position) = 0;

    std::shared_ptr<Dungeon> dungeon;
};


