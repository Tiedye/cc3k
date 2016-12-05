#include "EatAction.h"

int EatAction::getRange(const Character &source) const {
    return 1;
}

EatAction::EatAction() : Action(Type::CONSUME, "Eat", ACTION, EXTERNAL | INVENTORY, ANY, SQUARE, 0, 0) {}
