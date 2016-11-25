#include "InteractAction.h"

InteractAction::InteractAction():Action(Type::INTERACT, ACTION, ALL) {}

int InteractAction::getRange(Character &source) const {
    return 1;
}
