#include "WalkAction.h"

#include "../entity/Character.h"

WalkAction::WalkAction() :Action(Type::MOVE, MOVE) {

}

int WalkAction::getRange(Character &source) const {
    return source->getSpeed();
}

int WalkAction::getMinRange(Character &source) const {
    return 1;
}
