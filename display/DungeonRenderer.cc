#include "DungeonRenderer.h"

void DungeonRenderer::notify(EventInfo &info) {
    switch (info.eventType) {
        case MOVE_DONE:
        case MOVED_DONE:
            entityMoved(info.primary->asEntity(), info.eventPosition);
            break;
        case ADDED_TO_FLOOR_DONE:
            entityAdded(info.primary->asEntity());
            break;
        case REMOVED_FROM_FLOOR_DONE:
            entityRemoved(info.primary->asEntity());
            break;
        case ATTACK_DONE:
            entityAttacked(info.primary->asCharacter(), info.secondary->asEntity(), info.eventInteger);
            break;
        case HEAL_DONE:
            entityHealed(info.primary->asCharacter(), info.secondary->asEntity(), info.eventInteger);
            break;
        default:
            break;
    }
}

void DungeonRenderer::changeCell(Position position) {
    cellChanged(position);
}

const std::vector<EventType> DungeonRenderer::eventTypes {MOVE_DONE, MOVED_DONE, ADDED_TO_FLOOR_DONE, REMOVED_FROM_FLOOR_DONE, ATTACK_DONE, HEAL_DONE};

const std::vector<EventType> DungeonRenderer::listeningFor() const {
    return eventTypes;
}

void DungeonRenderer::setDungeon(const std::shared_ptr<Dungeon> &dungeon) {
    DungeonRenderer::dungeon = dungeon;
}
