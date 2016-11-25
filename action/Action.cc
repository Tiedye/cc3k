#include "Action.h"

Action::Action(const Type actionType, const Step type = PASS, const Action::Targets targets, const Action::Shape shape, const bool aoe,
               const int aoeSize) : type{type}, actionType(actionType), targets(targets), shape(shape), aoe(aoe),
                                    aoeSize(aoeSize) {}

int Action::getRange(Character &source) const {
    return 0;
}

void Action::onUse(std::shared_ptr<Character> source, std::shared_ptr<Entity> &target, Position castPoint) {}

void Action::onUse(std::shared_ptr<Character> source, Position castPoint) {}

int Action::getAmount(Character &source) {
    return 0;
}

int Action::getMinRange(Character &source) const {
    return 0;
}
