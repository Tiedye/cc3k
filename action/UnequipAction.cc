#include "UnequipAction.h"

int UnequipAction::get_range(const Character &source) const {
    return 1;
}

UnequipAction::UnequipAction(): Action(Type::UNEQUIP, "Unequip", ACTION, EXTERNAL | INVENTORY, ANY, SQUARE, 0, 0) {

}