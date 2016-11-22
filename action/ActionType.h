#pragma once

enum ActionType {
    PASS, // do nothing
    MOVE, // any kind of movement
    CONSUME, // any kind of consuming action
    EQUIP, // any kind of equipping action
    INTERACT, // any kind of interaction (no effect)
    ATTACK, // any kind of negative effect
    AID, // any kind of positive effect
    EFFECT // any remaining effects
};

// by default, players will be provided with these actions:
//  pass(PASS)
//  walk(MOVE)
//  interact(INTERACT)
//  consume(CONSUME)
//