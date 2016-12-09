#include "EquipAction.h"

int EquipAction::get_range(const Character &source) const {
    return 1;
}

EquipAction::EquipAction(): Action(Type::EQUIP, "Equip", Action::ACTION, EXTERNAL | INVENTORY, ANY, SQUARE, 0, 0) {

}
