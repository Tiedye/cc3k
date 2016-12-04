#include "State.h"

#include <random>

State::State() : gen(std::random_device()()) {
}
