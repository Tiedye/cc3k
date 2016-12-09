#include "DungeonRenderer.h"

void DungeonRenderer::notify(EventInfo &info) {
    switch (info.event_type) {
        case MOVE_DONE:
        case MOVED_DONE:
            entity_moved(info.primary->as_entity(), info.event_position);
            break;
        case ADDED_TO_FLOOR_DONE:
            entity_added(info.primary->as_entity());
            break;
        case REMOVED_FROM_FLOOR_DONE:
            entity_removed(info.primary->as_entity());
            break;
        case ATTACK_DONE:
            entity_attacked(info.primary->as_character(), info.secondary->as_entity(), info.event_integer);
            break;
        case MISS_DONE:
            entity_missed(info.primary->as_character(), info.secondary->as_entity());
            break;
        case HEAL_DONE:
            entity_healed(info.primary->as_character(), info.secondary->as_entity(), info.event_integer);
            break;
        default:
            break;
    }
}

void DungeonRenderer::change_cell(Position position) {
    cell_changed(position);
}

const std::vector<EventType> DungeonRenderer::event_types {MOVE_DONE, MOVED_DONE, ADDED_TO_FLOOR_DONE, REMOVED_FROM_FLOOR_DONE, ATTACK_DONE, MISS_DONE, HEAL_DONE};

const std::vector<EventType> DungeonRenderer::listening_for() const {
    return event_types;
}

void DungeonRenderer::set_dungeon(const std::shared_ptr<Dungeon> &dungeon) {
    DungeonRenderer::dungeon = dungeon;
}

DungeonRenderer::DungeonRenderer() : Listener(10000) {}
