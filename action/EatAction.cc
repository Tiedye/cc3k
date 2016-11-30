#include "EatAction.h"

int EatAction::getRange(Character &source) const {
    return 1;
}

EatAction::EatAction() : Action(Type::CONSUME, ACTION, EXTERNAL|INVENTORY) {}
