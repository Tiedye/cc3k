#include "StateConditional.h"

#include "../State.h"

StateConditional::StateConditional(const int state_id, const std::shared_ptr<Controller> &true_controller, const std::shared_ptr<Controller> &false_controller) : state_id(state_id),
                                                                                                                                                               true_controller(
                                                                                                                                                                       true_controller),
                                                                                                                                                               false_controller(
                                                                                                                                                                       false_controller) {}

const Controller::ActionAndTarget
StateConditional::get_action(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    return state->ai_get_integer(state_id) ? true_controller->get_action(character, actions, state) : false_controller->get_action(character, actions, state);
}
