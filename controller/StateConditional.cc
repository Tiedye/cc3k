#include "StateConditional.h"

#include "../State.h"

StateConditional::StateConditional(const int stateId, const std::shared_ptr<Controller> &trueController, const std::shared_ptr<Controller> &falseController) : stateId(stateId),
                                                                                                                                                               trueController(
                                                                                                                                                                       trueController),
                                                                                                                                                               falseController(
                                                                                                                                                                       falseController) {}

const Controller::ActionAndTarget
StateConditional::getAction(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    return state->aiGetInteger(stateId) ? trueController->getAction(character, actions, state) : falseController->getAction(character, actions, state);
}
