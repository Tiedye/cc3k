#include "Action.h"

Action::Action(const Type action_type, const std::string name, const Step type, const int targets, const Range range, const Shape shape, const bool aoe, const int aoe_size)
        : type{type}, action_type(action_type), name{name}, targets(targets), range(range), shape(shape), aoe(aoe),
          aoe_size(aoe_size) {}

int Action::get_range(const Character &source) const {
    return 0;
}

void Action::on_use(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, Position cast_point) {}

void Action::on_use(const std::shared_ptr<Character> &source, Position cast_point) {}

int Action::get_amount(Character &source) const {
    return 0;
}

int Action::get_min_range(const Character &source) const {
    return 0;
}

bool Action::can_target(const Position position, const std::shared_ptr<State> &state) {
    return true;
}
