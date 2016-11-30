#include "InteractAction.h"

InteractAction::InteractAction():Action(Type::INTERACT, ACTION, EXTERNAL|INVENTORY) {}

int InteractAction::getRange(Character &source) const {
    return 1;
}
