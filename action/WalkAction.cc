#include "WalkAction.h"

#include "../entity/Character.h"

WalkAction::WalkAction() :Action(MOVE, MOVEACTION) {

}

int WalkAction::getRange(Character &source) const {
    return source.getSpeed();
}

int WalkAction::getMinRange(Character &source) const {
    return 1;
}
