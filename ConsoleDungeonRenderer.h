#pragma once


#include "display/DungeonRenderer.h"

class ConsoleDungeonRenderer : public DungeonRenderer {
public:
    using DungeonRenderer::DungeonRenderer;
protected:
    virtual void entityMoved(const Entity *entity, Position oldPos) override;

    virtual void entityCreated(const Entity *entity) override;

    virtual void entityDestroyed(const Entity *entity) override;

    virtual void cellChanged(Position position) override;
};


