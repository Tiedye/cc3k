#include "Action.h"

Action::Action(const Type actionType, const Step type, const int targets, const Range range, const Action::Shape shape,
               const bool aoe,
               const int aoeSize) : type{type}, actionType(actionType), targets(targets), range(range), shape(shape), aoe(aoe),
                                    aoeSize(aoeSize) {}

int Action::getRange(Character &source) const {
    return 0;
}

void Action::onUse(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, Position castPoint) {}

void Action::onUse(const std::shared_ptr<Character> &source, Position castPoint) {}

int Action::getAmount(Character &source) const {
    return 0;
}

int Action::getMinRange(Character &source) const {
    return 0;
}

bool Action::canTarget(const Position position, const std::shared_ptr<State> &state) {
    return true;
}
