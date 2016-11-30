#pragma once


#include "../event/EventType.h"
#include "../util/Position.h"

#include <memory>

class Entity;

class Character;

class Action {
public:
    enum Targets {
        SELF = 1, EXTERNAL = 0b10, INVENTORY = 0b100
    };
    enum Shape {
        POINT, LINE, SQUARE
    };
    enum Range {
        ANY, LOS, PATH
    };
    enum Step {
        MOVEACTION, ACTION, BONUSACTION, PASSACTION
    };
    enum Type {
        PASS, // do nothing
        MOVE, // any kind of movement
        CONSUME, // any kind of consuming action
        EQUIP, // any kind of equipping action
        UNEQUIP, // any kind of equipping action
        INTERACT, // any kind of interaction (no effect)
        ATTACK, // any kind of negative effect
        AID, // any kind of positive effect
        EFFECT // any remaining effects
    };

    const Step type;

    const Type actionType;

    const int targets;
    const Range range;

    const Shape shape;
    const bool aoe;
    const int aoeSize;

    virtual int getRange(Character &source) const;

    virtual int getMinRange(Character &source) const;

    // used to get values associated with ATTACK and AID actions, can also be used before that for informational purposes
    virtual int getAmount(Character &source) const;
    virtual void onUse(const std::shared_ptr<Character> &source,
                       Position castPoint); // called on actions of type: PASS, MOVE, AID(SELF), EFFECT(SELF)
    virtual void onUse(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target,
                       Position castPoint); // called on actions of type: CONSUME, EQUIP, INTERACT, ATTACK, AID(!SELF), EFFECT(!SELF)


protected:
    Action(const Type actionType, const Step type = PASSACTION, const int targets = SELF, const Range range = ANY,
           const Shape shape = POINT, const bool aoe = false, const int aoeSize = 0);
};



