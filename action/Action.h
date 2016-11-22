#pragma once


#include "../util/EventType.h"
#include "../util/Position.h"

#include <memory>

class Entity;
class Character;

struct Action {
    const EventType preEvent;
    const EventType postEvent;

    enum Targets { SELF, FRIENDLY, HOSTILE, ALL };
    const Targets targets;
    const int range;
    enum Shape { POINT, LINE, CONE, CIRCLE, SQUARE};
    const Shape shape;
    const bool aoe;
    const int aoeSize;

    Action(EventType preEvent, EventType postEvent, Targets targets = SELF, int range = 0, Shape shape = POINT, bool aoe = false, int aoeSize = 0);

    virtual void onHit(std::shared_ptr<Character> from, std::shared_ptr<Entity> to, Position castPoint) = 0;
};



