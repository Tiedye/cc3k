#pragma once


#include "DungeonRenderer.h"

class ConsoleDungeonRenderer : public DungeonRenderer {
public:
    using DungeonRenderer::DungeonRenderer;
protected:
    virtual void entityMoved(const std::shared_ptr<Entity> entity, Position oldPos) override;

    virtual void entityCreated(const std::shared_ptr<Entity> entity) override;

    virtual void entityDestroyed(const std::shared_ptr<Entity> entity) override;

    virtual void cellChanged(Position position) override;
};


