#include "DungeonRenderer.h"

void DungeonRenderer::notify(EventInfo info) {
    switch (info.eventType) {
        case MOVE_DONE:
        case MOVED_DONE:
            entityMoved(info.primary->asEntity(), info.eventPosition);
            break;
        case CREATED_DONE:
            entityCreated(info.primary->asEntity());
            break;
        case DESTROYED_DONE:
            entityDestroyed(info.primary->asEntity());
            break;
        default:
            break;
    }
}

bool DungeonRenderer::isListeningFor(EventType type) {
    switch (type) {
        case MOVE_DONE:
        case MOVED_DONE:
        case DESTROYED_DONE:
        case CREATED_DONE:
            return true;
        default:
            return false;
    }
}

void DungeonRenderer::changeCell(Position position) {
    cellChanged(position);
}

DungeonRenderer::DungeonRenderer(const std::vector<CellType> &cells):cells{cells} {}
