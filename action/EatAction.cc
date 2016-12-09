#include "EatAction.h"

int EatAction::get_range(const Character &source) const {
    return 1;
}

EatAction::EatAction() : Action(Type::CONSUME, "Eat", ACTION, EXTERNAL | INVENTORY, ANY, SQUARE, 0, 0) {}
