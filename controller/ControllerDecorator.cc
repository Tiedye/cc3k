#include "ControllerDecorator.h"

ControllerDecorator::ControllerDecorator(const std::shared_ptr<Controller> &decorated) : decorated(decorated) {}
