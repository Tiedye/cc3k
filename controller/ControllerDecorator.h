#pragma once


#include "Controller.h"

class ControllerDecorator: public Controller {
public:
    ControllerDecorator(const std::shared_ptr<Controller> &decorated);
protected:
    std::shared_ptr<Controller> decorated;
};


