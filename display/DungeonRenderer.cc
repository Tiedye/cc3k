#include "DungeonRenderer.h"

void DungeonRenderer::notify(EventInfo &info) {
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

void DungeonRenderer::changeCell(Position position) {
    cellChanged(position);
}

DungeonRenderer::DungeonRenderer(const std::vector<CellType> &cells):cells{cells} {}

std::vector<EventType> DungeonRenderer::eventTypes {MOVE_DONE, MOVED_DONE, CREATED_DONE, DESTROYED_DONE};

const std::vector<EventType> DungeonRenderer::listeningFor() const {
    return eventTypes;
}
