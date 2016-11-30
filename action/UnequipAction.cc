#include "UnequipAction.h"

int UnequipAction::getRange(Character &source) const {
    return 1;
}

UnequipAction::UnequipAction():Action(Type::UNEQUIP, BONUSACTION, EXTERNAL|INVENTORY) {

}