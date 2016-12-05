#include "InteractAction.h"

InteractAction::InteractAction(): Action(Type::INTERACT, "Pickup/Interact", ACTION, EXTERNAL | INVENTORY, ANY, SQUARE, 0, 0) {}

int InteractAction::getRange(const Character &source) const {
    return 1;
}
