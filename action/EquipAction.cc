#include "EquipAction.h"

int EquipAction::getRange(Character &source) const {
    return 1;
}

EquipAction::EquipAction():Action(Type::EQUIP, BONUSACTION, EXTERNAL|INVENTORY) {

}
