#include "WalkAsActionAction.h"

#include "../entity/Character.h"

WalkAsActionAction::WalkAsActionAction():Action(MOVE, ACTION, SELF, STRICT_PATH) {

}

int WalkAsActionAction::getRange(Character &source) const {
    return source.getSpeed();
}

int WalkAsActionAction::getMinRange(Character &source) const {
    return 1;
}
