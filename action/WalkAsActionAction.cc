#include "WalkAsActionAction.h"

#include "../entity/Character.h"

WalkAsActionAction::WalkAsActionAction():Action(MOVE, ACTION) {

}

int WalkAsActionAction::getRange(Character &source) const {
    return source.getSpeed();
}

int WalkAsActionAction::getMinRange(Character &source) const {
    return 0;
}
