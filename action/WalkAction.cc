#include "WalkAction.h"

#include "../entity/Character.h"

WalkAction::WalkAction() :Action(MOVE, MOVEACTION, SELF, STRICT_PATH) {

}

int WalkAction::getRange(Character &source) const {
    return source.getSpeed();
}

int WalkAction::getMinRange(Character &source) const {
    return 1;
}
